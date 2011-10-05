#!/usr/bin/perl
use warnings; 
use strict;
use threads;
use threads::shared;
use IO::Select;
use IO::Pipe;

my @ranges = ( [1,100000],[1000001,200000],[200001,300000],
               [300001,400000],[400001,500000] );

my $sel = new IO::Select();

# thread launching

my $count:shared; # a simple shared counter to detect
$count = 0;       # when all detached threads have exited

foreach (@ranges)
{
	my $pipe  = IO::Pipe->new();
	my $start = $_->[0];
	my $end   = $_->[1];
	
	#print "$start $end $pipe\n";
	threads->create( \&thread, $start, $end, $pipe )->detach;
	
	$count++;
	
	# only call reader after pipe has been passed to thread  
	$sel->add( $pipe->reader() );
}

# watching thread output
print "Watching\n\n";

while(1)
{
	# use a timeout appropriate for the task
	# as it will trigger the exit test, measured in seconds
	foreach my $h ($sel->can_read(1) )
	{
		my $buf;
		if ( (sysread($h, $buf, 4096) > 0 ) )
		{
			print "Main says: $buf\n"; 
		}
		if ($count == 0)
		{	
			print "Done\n"; 
			exit;
		}
	}
}


sub thread
{
	my( $start, $finish, $pipe ) = @_;
	
	my $wh = $pipe->writer;
	$wh->autoflush(1);
	
	print $wh "thread# ",threads->tid()," -> $start, $finish, $pipe \n";
	
	foreach my $num ($start .. $finish)
	{
		if ( is_prime($num) )
		{
			print $wh "thread #",threads->tid()," ->$num\n";
		}
	}
	
	sub is_prime 
	{
		# from a Merlyn column
		my $guess = shift;
		for (my $divisor = 2; $divisor * $divisor <= $guess; $divisor++)
		{
			return unless $guess % $divisor;
		}
		return 1;
	}
	
	print $wh  "\t\t\t\t\t\t thread# ",threads->tid()," -> finishing \n"  ;
	
	sleep 1;  # let pipe flush buffers
	
	$count--;

}

__END__
