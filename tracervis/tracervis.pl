#!/usr/bin/perl

use strict;
use warnings;
use Getopt::Long;
use Pod::Usage;
use DBI;


my $file_prefix = 'C:\\Users\\m044910\\myrimatch\\';
my $man = 0;
my $help = 0;
my $db = '';
# my $port = 0;
# my $host = "";

GetOptions(
           'help|?' => \$help,
           'man' => \$man,
           'db:s' => \$db,
#            'p|port=i' => \$port,
#            'a:s' => \$host
          ) or pod2usage(-verbose => 2);

pod2usage(1) if $help;
if($db eq '') {
	print STDERR "Missing -db parameter";
}
pod2usage(-verbose => 2) if $man;
my $dbh;

my %codes = (); # Map from filename(linenum) to id of the code reference


my $sth_insert_code; # Insert new code reference 
my $sth_insert_op; # Insert new operation
my $sth_insert_object; 
my $sth_insert_io;

&main; exit;

my @op_stack = (); # Stack of operations

my $object_id = 0; # Object id counter, incremented on each new object encountered

my @mem_stack = (); # Stack. Each level contains an array of all object memory addresses allocated in that stack frame
my %objects = (); # Map from MEM address -> object id

# Object is defined by memory location + type
sub is_defined_object {
    my ($mem, $type) = @_;
    return defined $objects{"$mem:$type"};
}

sub define_object {
    my ($mem, $type, $obj, $stack) = @_;
    $objects{"$mem:$type"} = $obj;
    if($stack) {
        $mem_stack[-1]{"$mem:$type"} = 1;
    }
}

sub get_object {
    my ($mem, $type) = @_;
    return $objects{"$mem:$type"};
}

sub undefine_object {
    my ($mem, $type) = @_;
    delete $objects{"$mem:$type"};
}

sub add_code {
	my ($id, $file, $line) = @_;
	defined $file || die "$id: File not defined";
	if (uc(substr($file, 0, length($file_prefix))) eq uc($file_prefix)) {
		$file = substr($file, length($file_prefix));
	}
	$file =~ s/\\/\//g;
	my $key = "$file($line)";
	if(defined $codes{$key}) {
		return $codes{$key};
	} 
	$codes{$key} = $id;
	$sth_insert_code->execute($id, $file, $line) || die "Could not insert code reference: " . $dbh->errstr;	
	return $id;
}

sub op_start {
	my ($id, $name, $file, $line) = @_;
	my $code = add_code($id, $file, $line);

	my %op = ();
	$op{'id'} = $id;
	$op{'name'} = $name;
	$op{'code_start_id'} = $code;
	$op{'parent_id'} = defined $op_stack[-1] ? $op_stack[-1]->{'id'} : -1;
        push(@op_stack, \%op);	
}

sub op_end {
	my ($id, $name, $file, $line) = @_;
	my $code = add_code($id, $file, $line);
	
	my $op = pop(@op_stack);
	($op->{'name'} eq $name) || die "Operation mismatch open/close \nOPEN: \t".$op->{'name'}."\nCLOSE:\t".$name;
	$op->{'code_end_id'} = $code;
	$op->{'terminated_time'} = $id;
	$sth_insert_op->execute($op->{'id'}, $op->{'name'}, $op->{'parent_id'}, $op->{'code_start_id'}, $op->{'code_end_id'}, $op->{'terminated_time'}) || die "Couldn't insert operation: ".$dbh->errstr;
}

# Return id of the IO which accessed the object
sub add_object {
	my ($id, $name, $heap, $operation, $note, $code, $mem, $type, $value, $parent_io, $parent_relation) = @_;

    my $obj;
    if(is_defined_object($mem, $type)) {
        $obj = get_object($mem, $type);
    } else {
        $object_id++;
        $obj = { 'id' => $object_id, 'allocated_time' => $id, 'type' => $type };
        define_object($mem, $type, $obj, $heap eq '2');
    }
    $sth_insert_io->execute($id, $obj->{'id'}, $op_stack[-1]->{'id'}, $name, $value, $operation, $note, $code, $parent_io, $parent_relation) || die "Couldn't add io: " . $dbh->errstr;
    return $dbh->last_insert_id("","","","");
}

sub display_serialization {
    my($offset, $tab, $serialization_ref) = @_;
    my @serialization = @{$serialization_ref};
    if(scalar @serialization == 0) { return -1; }
    if((scalar @serialization)<=$offset) {
        print STDERR "offset problem $offset : " . display_serialization(0, \@serialization) . "\n";
        return $offset;
    }
    my $lookup = $serialization[$offset++];

    if($lookup eq '.') { # Literal
        my $mem = $serialization[$offset++];
        my $type = $serialization[$offset++];
        my $value = $serialization[$offset++];
        print STDERR "$tab object $mem $type $value\n";
    } elsif($lookup eq '[') { # Array or structure
        my $mem = $serialization[$offset++];
        my $type = $serialization[$offset++];
        print STDERR "$tab struct $mem $type\n";
        while($serialization[$offset] ne ']') {
            my $member = $serialization[$offset++];
            print "$tab $member:\n";
            $offset = display_serialization($offset, "\t".$tab, $serialization_ref);
        }
        $offset++; # Eat up the closing bracket
    } elsif($lookup eq '1{') {
        my $mem = $serialization[$offset++];
        my $type = $serialization[$offset++];
        print STDERR "$tab hash $mem $type\n";

        my $i=0;
        while($serialization[$offset] ne '}') {
            $offset = display_serialization($offset, "\t".$tab, $serialization_ref);
            $offset = display_serialization($offset, "\t".$tab, $serialization_ref);
            $i++;
        }
        $offset++; # Eat up the closing brace
    }
    return $offset;

}

# Parse a serialized object into all sub-objects
# Returns offset in the serialization array with first unparsed element
sub parse_obj {
	my ($id, $name, $heap, $operation, $note, $code, $parent_io, $parent_relation, $offset, $serialization_ref) = @_;
    my @serialization = @{$serialization_ref};
    if(scalar @serialization == 0) { return -1; }
    if((scalar @serialization)<=$offset) {
        print STDERR join("\t", @serialization) . "\n";
        print STDERR "----\n";
        print STDERR "offset problem $offset : " . display_serialization(0, '', \@serialization) . "\n";
    }
    my $lookup = $serialization[$offset++];

    if($lookup eq '.') { # Literal
        my $mem = $serialization[$offset++];
        my $type = $serialization[$offset++];
        my $value = $serialization[$offset++];
        add_object($id, $name, $heap, $operation, $note, $code, $mem, $type, $value, $parent_io, $parent_relation);
    } elsif($lookup eq '[') { # Array or structure
        my $mem = $serialization[$offset++];
        my $type = $serialization[$offset++];
        my $arr_io = add_object($id, $name, $heap, $operation, $note, $code, $mem, $type, '', $parent_io, $parent_relation);
        while($serialization[$offset] ne ']') {
            my $member = $serialization[$offset++];
            $offset = parse_obj($id, $name, $heap, $operation, $note, $code, $arr_io, $member, $offset, $serialization_ref);
        }
        $offset++; # Eat up the closing bracket
    } elsif($lookup eq '{') {
        my $mem = $serialization[$offset++];
        my $type = $serialization[$offset++];

        my $map_io = add_object($id, $name, $heap, $operation, $note, $code, $mem, $type, '', $parent_io, $parent_relation);
        my $i=0;
        while($serialization[$offset] ne '}') {
            $offset = parse_obj($id, $name, $heap, $operation, $note, $code, $map_io, "$i key", $offset, $serialization_ref);
            $offset = parse_obj($id, $name, $heap, $operation, $note, $code, $map_io, "$i value", $offset, $serialization_ref);
            $i++;
        }
        $offset++; # Eat up the closing brace
    }
    return $offset;
}

sub tr_dump {
	my ($id, $name, $heap, $operation, $note, $file, $line, @serialization) = @_;
	# $heap - 1-heap, 2-stack
	# $operation - 1-read, 2-write, 3-note

	my $code = add_code($id, $file, $line);

           # ($id, $name, $heap, $operation, $note, $code, $parent_id, $parent_relation, $offset, $serialization_ref)
	parse_obj($id, $name, $heap, $operation, $note, $code, -1,         '',               0,       \@serialization);
}

sub tr_ref {
	my ($id, $mem, $name, $heap, $operation, $note, $file, $line) = @_;
}

# Deallocate object on leaving a stack frame or when calling delete on heap
# Need to deallocate members of a group
sub deallocate {
	my($id, $mem, $type) = @_;
	is_defined_object($mem, $type) || die "$id: The object $type at memory address $mem was not defined.\n";
	my $object = get_object($mem, $type);
	$object->{'deallocated_time'} = $id;
	$sth_insert_object->execute($object->{'id'}, $object->{'type'}, $object->{'allocated_time'}, $object->{'deallocated_time'});
	undefine_object($mem, $type);
}

# Stackframe starts
sub block_in {
	my ($id) = @_;
	my %objs = ();
	push(@mem_stack, \%objs);
}

# Stackframe ends
sub block_out {
	my ($id) = @_;
	my $objs = pop(@mem_stack);
	foreach my $obj(keys %{$objs}) {
	    my ($mem, $type) = split(/:/, $obj, 2);
		deallocate($id, $mem, $type);
	}
}

sub main {
	# The id of each object is the line number where it first appeared
	$dbh = DBI->connect( "dbi:SQLite:$db" ) || die "Cannot connect: $DBI::errstr";
	$dbh->do( "CREATE TABLE IF NOT EXISTS operation (operation_id INTEGER PRIMARY KEY, name TEXT, parent_id INTEGER, code_start_id INTEGER, code_end_id INTEGER, terminated_time INTEGER)" ) || die "Couldn't create table: ". $dbh->errstr;
	$sth_insert_op = $dbh->prepare("INSERT INTO operation (operation_id, name, parent_id, code_start_id, code_end_id, terminated_time) VALUES (?, ?, ?, ?, ?, ?)") || die "Couldn't prepare statement: " . $dbh->error;
	$dbh->do( "CREATE TABLE IF NOT EXISTS code (code_id INTEGER PRIMARY KEY, file TEXT, line INTEGER)" ) || die "Couldn't create table: ". $dbh->errstr;
	$sth_insert_code = $dbh->prepare("INSERT INTO code (code_id, file, line) VALUES (?, ?, ?)") || die "Couldn't prepare statement: " . $dbh->errstr;
	$dbh->do("CREATE TABLE IF NOT EXISTS object (object_id INTEGER PRIMARY KEY, type TEXT, allocated_time INTEGER, deallocated_time INTEGER)") || die "Couldn't prepare statement: " . $dbh->error;
	$sth_insert_object = $dbh->prepare("INSERT INTO object (object_id, type, allocated_time, deallocated_time) VALUES (?, ?, ?, ?)") || die "Couldn't prepare statement: " . $dbh->errstr;
	$dbh->do("CREATE TABLE IF NOT EXISTS io (io_id INTEGER PRIMARY KEY AUTOINCREMENT, io_time INTEGER, object_id INTEGER, operation_id INTEGER, name TEXT, value TEXT, readwrite INTEGER, note TEXT, code_id INTEGER, parent_id INTEGER, parent_relation TEXT)") || die "Couldn't prepare statement: " . $dbh->error;
	$sth_insert_io = $dbh->prepare("INSERT INTO io (io_time, object_id, operation_id, name, value, readwrite, note, code_id, parent_id, parent_relation) VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?)") || die "Couldn't prepare statement: " . $dbh->errstr;

	my $linenum=0;

	op_start($linenum, "all", "", 0);
	block_in($linenum);

    my $fileLine = 0;
	while(my $line = <>) {
	    $fileLine++;
		chomp $line;
		if($line =~ /^\[TRACER\]\t([^\t]+)\t?(.*)$/) {			
			my $op = $1;
			my @params = split(/\t/, $2);
			$linenum++;
			if($op eq 'op_start') {
				op_start($linenum, @params);
			} elsif($op eq 'op_end') {
				op_end($linenum, @params);
			} elsif($op eq 'dump') {
				tr_dump($linenum, @params);
			} elsif($op eq 'ref') {
				tr_ref($linenum, @params); 
			} elsif($op eq 'block_in') {
				block_in($linenum, @params);
			} elsif($op eq 'block_out') {
				block_out($linenum, @params);
			}
		}
		if($linenum % 10000 == 0) {
			print STDERR "$linenum\n";
		}
	}

	block_out($linenum);	
	
	# Dump the rest of the heap
	foreach my $obj (keys %objects) {
	    my ($mem, $type) = split(/:/, $obj, 2);
		deallocate($linenum, $mem, $type);
	}
	op_end($linenum, "all", "", 0);
}



__END__

=head1 NAME

tracervis.pl - Visualize results of a Myrimatch trace

=head1 SYNOPSIS

 tracervis.pl [options] [file ...]

 Options:
   -help            brief help message
   -man             full documentation
   -db              sqlite3 database file to write the output into

=head1 OPTIONS

=over 2

=item B<-help>

Print a brief help message and exits.

=item B<-man>

Prints the manual page and exits.

=item B<-db>

The sqlite3 database file into which we write the processed data.

=back

=head1 DESCRIPTION

B<TracerVis> reads the input file, parses out all the output of a tracer
and generates a user-friendly visualization of the entire execution using HTML.

=head1 AUTHOR

Roman Zenka <zenka.roman@mayo.edu>

=cut
