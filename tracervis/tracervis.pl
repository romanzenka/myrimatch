#!/usr/bin/perl

use strict;
use warnings;
use Getopt::Long;
use Pod::Usage;

my $man = 0;
my $help = 0;
# my $port = 0;
# my $host = "";

GetOptions(
           'help|?' => \$help,
           'man' => \$man,
#            'p|port=i' => \$port,
#            'a:s' => \$host
          ) or pod2usage(-verbose => 2);

pod2usage(1) if $help;
pod2usage(-verbose => 2) if $man;


&main; exit;

sub main {
	while(my $line = <>) {
		if($line =~ /^\[TRACER\]/) {
			chomp $line;
			
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

=head1 OPTIONS

=over 2

=item B<-help>

Print a brief help message and exits.

=item B<-man>

Prints the manual page and exits.

=back

=head1 DESCRIPTION

B<TracerVis> reads the input file, parses out all the output of a tracer
and generates a user-friendly visualization of the entire execution using HTML.

=head1 AUTHOR

Roman Zenka <zenka.roman@mayo.edu>

=cut
