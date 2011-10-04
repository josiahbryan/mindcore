#!/usr/bin/perl
use strict;
use CPAN;
#system("yum install -y mysql mysql-devel mysql-client mysql-server");
my @deps = qw/
	Class::Inspector
	JE	
/;

foreach my $dep (@deps)
{
	print "Installing: $dep\n";
	CPAN::install($dep);
}
