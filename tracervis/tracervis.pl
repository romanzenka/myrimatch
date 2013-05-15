#!/usr/bin/perl

use strict;
use warnings;
use Getopt::Long;
use Pod::Usage;
use DBI;


my $file_prefix = 'c:\\poster\\myrimatch\\';
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

my @mem_stack = (); # Stack. Each level contains an array of all object memory addresses allocated in that stack frame
my %objects = (); # Map from MEM address -> object id

sub add_code {
	my ($id, $file, $line) = @_;
	defined $file || die "$id: File not defined";
	if (substr($file, 0, length($file_prefix)) eq $file_prefix) {
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
	$op{'parent_id'} = defined $op_stack[-1] ? $op_stack[-1]->{'id'} : 0;
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

sub tr_dump {
	my ($id, $mem, $type, $value, $name, $heap, $operation, $note, $file, $line) = @_;
	# $heap - 1-heap, 2-stack
	# $operation - 1-read, 2-write, 3-note

	my $code = add_code($id, $file, $line);
	my $obj;
	if(defined $objects{$mem}) {
		$obj = $objects{$mem};
	} else {
		$obj = { 'id' => $id, 'name' => $name, 'type' => $type };
		$objects{$mem} = $obj;
		if($heap eq '2') {
			# Stack: This object will get deallocated when leaving this stack frame
			$mem_stack[-1]{$mem} = 1; 
		}
	}
	$sth_insert_io->execute($id, $obj->{'id'}, $op_stack[-1]->{'id'}, $value, $operation, $note, $code) || die "Couldn't add io: " . $dbh->errstr;	
}

sub tr_ref {
	my ($id, $mem, $name, $heap, $operation, $note, $file, $line) = @_;
}

# Deallocate object on leaving a stack frame or when calling delete on heap
sub deallocate {
	my($id, $mem) = @_;
	defined $objects{$mem} || die "$id: The object at memory address $mem was not defined.\n";
	my $object = $objects{$mem};
	$object->{'deallocated_time'} = $id;
	$sth_insert_object->execute($object->{'id'}, $object->{'name'}, $object->{'type'}, $object->{'deallocated_time'});
	delete $objects{$mem}; 
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
		deallocate($id, $obj);
	}
}

sub main {
	# The id of each object is the line number where it first appeared
	$dbh = DBI->connect( "dbi:SQLite:$db" ) || die "Cannot connect: $DBI::errstr";
	$dbh->do( "CREATE TABLE IF NOT EXISTS operation (operation_id INTEGER PRIMARY KEY, name TEXT, parent_id INTEGER, code_start_id INTEGER, code_end_id INTEGER, terminated_time INTEGER)" ) || die "Couldn't create table: ". $dbh->errstr;
	$sth_insert_op = $dbh->prepare("INSERT INTO operation (operation_id, name, parent_id, code_start_id, code_end_id, terminated_time) VALUES (?, ?, ?, ?, ?, ?)") || die "Couldn't prepare statement: " . $dbh->error;
	$dbh->do( "CREATE TABLE IF NOT EXISTS code (code_id INTEGER PRIMARY KEY, file TEXT, line INTEGER)" ) || die "Couldn't create table: ". $dbh->errstr;
	$sth_insert_code = $dbh->prepare("INSERT INTO code (code_id, file, line) VALUES (?, ?, ?)") || die "Couldn't prepare statement: " . $dbh->errstr;
	$dbh->do("CREATE TABLE IF NOT EXISTS object (object_id INTEGER PRIMARY KEY, name TEXT, type TEXT, deallocated_time INTEGER)") || die "Couldn't prepare statement: " . $dbh->error;
	$sth_insert_object = $dbh->prepare("INSERT INTO object (object_id, name, type, deallocated_time) VALUES (?, ?, ?, ?)") || die "Couldn't prepare statement: " . $dbh->errstr;	
	$dbh->do("CREATE TABLE IF NOT EXISTS io (io_id INTEGER PRIMARY KEY, object_id INTEGER, operation_id INTEGER, value TEXT, readwrite INTEGER, note TEXT, code_id INTEGER)") || die "Couldn't prepare statement: " . $dbh->error;
	$sth_insert_io = $dbh->prepare("INSERT INTO io (io_id, object_id, operation_id, value, readwrite, note, code_id) VALUES (?, ?, ?, ?, ?, ?, ?)") || die "Couldn't prepare statement: " . $dbh->errstr;	

	my $linenum=0;

	op_start($linenum, "all", "", 0);
	block_in($linenum);

	while(my $line = <>) {
		chomp $line;
		if($line =~ /^\[TRACER\]\t([^\t]+)\t(.*)$/) {			
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
			}
		}
	}

	block_out($linenum);	
	
	# Dump the rest of the heap
	foreach my $mem (keys %objects) {
		deallocate($linenum, $mem);
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
