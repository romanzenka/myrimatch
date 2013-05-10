#!/usr/bin/perl

use strict;
use warnings;
use Getopt::Long;
use Pod::Usage;
use DBI;


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

&main; exit;

my $op_level = 0;
my $prefix = '';

sub op_start {
	my ($id, $name, $file, $line) = @_;
	$op_level++;
	print "$prefix  / $name\t\t\t\t$file($line)\n";
	$prefix .= '  |';
}

sub op_end {
	my ($id, $name, $file, $line) = @_;
	$op_level--;
	$prefix = substr($prefix, 0, length($prefix) - 3);
	print "$prefix   \\ $name \t\t\t\t$file($line)\n";
}

sub tr_label {
	my ($id, $mem, $label) = @_;
	print "$prefix label $mem $label\n";
}

sub tr_var {
	my ($id, $mem, $name, $file, $line) = @_;
	print "$prefix     $mem $name \t\t\t\t$file($line)\n";
}

sub tr_start {
	my ($id, $mem) = @_;
	print "$prefix start $mem\n";
}

sub tr_dump {
	my ($id, $mem, $type, $value) = @_;
	print "$prefix     $mem ($type) \t";
	if(defined $value) {
		print "$value";
	} else {
		print "(empty)";
	}
	print "\n";
}

sub tr_reason {
	my ($id, $mem, $reason) = @_;
	print "$prefix reason $mem $reason\n";
}

sub tr_op_in {
	my ($id, $mem) = @_;
	print "$prefix<--- $mem\n";
}

sub tr_op_out {
	my ($id, $mem) = @_;
	print "$prefix---> $mem\n";
}

sub main {
	# The id of each object is the line number where it first appeared
	$dbh = DBI->connect( "dbi:SQLite:$db" ) || die "Cannot connect: $DBI::errstr";
	$dbh->do( "CREATE TABLE operation (operation_id INTEGER PRIMARY KEY, name TEXT, parent_id INTEGER, code_start_id INTEGER, code_end_id INTEGER, terminated_time INTEGER)" );
	$dbh->do( "CREATE TABLE input (input_id INTEGER PRIMARY KEY, operation_id INTEGER, object_id INTEGER, code_id INTEGER)" ); 
	$dbh->do( "CREATE TABLE output (output_id INTEGER PRIMARY KEY, operation_id INTEGER, object_id INTEGER, code_id INTEGER)" );
	$dbh->do( "CREATE TABLE object (object_id INTEGER PRIMARY KEY, memory_address INTEGER, terminated_time INTEGER)" );
	$dbh->do( "CREATE TABLE label (object_id INTEGER PRIMARY KEY, time INTEGER, text TEXT)" );	
	$dbh->do( "CREATE TABLE dump (object_id INTEGER PRIMARY KEY, time INTEGER, value TEXT)" );	
	$dbh->do( "CREATE TABLE reason (object_id INTEGER PRIMARY KEY, time INTEGER, reason TEXT)" );	
	$dbh->do( "CREATE TABLE code (code_id INTEGER PRIMARY KEY, file TEXT, line INTEGER)" );

	my $linenum=0;

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
			} elsif($op eq 'label') {
				tr_label($linenum, @params);
			} elsif($op eq 'var') {
				tr_var($linenum, @params);
			} elsif($op eq 'start') {
				tr_start($linenum, @params);				
			} elsif($op eq 'dump') {
				tr_dump($linenum, @params);				
			} elsif($op eq 'reason') {
				tr_reason($linenum, @params);				
			} elsif($op eq 'op_in') {
				tr_op_in($linenum, @params);				
			} elsif($op eq 'op_out') {
				tr_op_out($linenum, @params);				
			}
		}
	}
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
