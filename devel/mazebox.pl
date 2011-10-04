#!/usr/bin/perl
use strict;
use lib '../lib';
use MindCore qw/:namespace :types _node/;
use Data::Dumper;
use Time::HiRes qw/time sleep/;
use Term::ANSICanvas;

#print STDERR "Test\n";

	my $bg = Term::ANSICanvas->new(10,20);
	$bg->fill([ON_YELLOW,BOLD],0,0,$bg->height,$bg->width,' ');
	$bg->box(1,0,$bg->height-1,$bg->width-1,ON_YELLOW,BOLD,0);
	
	my $c2 = Term::ANSICanvas->new(5,5);
	#$c2->print("12345\n54321");
	#$c2->fill([ON_GREEN,BOLD],0,0,$c2->height,$c2->width,-1);
	#$c2->box(0,0,$c2->height-2,$c2->width-2,ON_WHITE,BLACK,1);
	#$c2->putchr(0,0,"X");
	$c2->string(0,0,'<white><onblue>#<clear>');
		
	my $can = Term::ANSICanvas->new($bg->height,$bg->width);

	

	my $x =0;
	my $y =0;
	my $ox = 3;
	my $oy = 3;
		
		
	my $dy = 1;
	my $dx = 1;
	
	my $count =0;
	
	my $a = time;
	while(++$count<350)
	{
		
		$can->clear;
		#$can->line([ON_BLUE,RED],$y,$x,$y+($oy * sign($dy)),$x+($ox * sign($dx)),' ');
		
		$can->render_canvas(0,0,$bg);
		$can->render_canvas($y,$x,$c2);
		
		if($can->getchr($y+$dy*2,$x+$dx*2) ne ' ')
		{
			$dx *= -1 * int(rand()*2+1);
			$dy *= -1 * int(rand()*2+1);
			
		}
		
		
		$x+=$dx;
		$y+=$dy;
		
		
		$x=$y=2 if $x > $can->width-1 || $y > $can->height-1 || $x<0|| $y<0;
		$dx = -1 if($x>$can->width-3);
		$dy = -1 if($y>$can->height-3);
		$dx = 1 if $x<3;
		$dy = 1 if $y<3;
		
		
		print CURSOR_OFF . $can->to_string(0,0,'.') . CURSOR_ON;
		
		
		my $b = time;
		my $d = $b-$a;
		my $fps = $count/$d;
		
		print "\n$y,$x           \n($dx,$dy)              \n(".$can->height.",".$can->width.") [frame $count] [".sprintf("%.02f fps",$fps)."]      \n";
		
		
		sleep 1/10;
	}
	
	my $b = time;
	my $d = $b-$a;
	my $fps = $count/$d;
	
	print "\n".REVERSE().'[ Demo3 ]'.CLEAR()." >> [ FPS Test ]\n";
	print "d=$d, fps=$fps\n";
	