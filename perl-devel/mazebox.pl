#!/usr/bin/perl
use strict;
use lib '../lib';
use MindCore qw/:namespace :types _node/;
use Data::Dumper;
use Time::HiRes qw/time sleep/;
use Term::ANSICanvas;

#print STDERR "Test\n";

	my $bg = Term::ANSICanvas->new(20,40);
	$bg->fill([ON_YELLOW,BOLD],0,0,$bg->height,$bg->width,' ');
	$bg->box(1,0,$bg->height-1,$bg->width-1,ON_YELLOW,BOLD,0);
	$bg->box(5,20,10,26,ON_YELLOW,BOLD,0);
	$bg->box(3,10,4,16,ON_YELLOW,BOLD,0);
	$bg->box(15,5,16,35,ON_YELLOW,BOLD,0);
	
	$bg->string(6,6,'<white><onred>X<clear>');
	
	my $c2 = Term::ANSICanvas->new(1,1);
	#$c2->print("12345\n54321");
	#$c2->fill([ON_GREEN,BOLD],0,0,$c2->height,$c2->width,-1);
	#$c2->box(0,0,$c2->height-2,$c2->width-2,ON_WHITE,BLACK,1);
	#$c2->putchr(0,0,"X");
	$c2->string(0,0,'<white><onblue>#<clear>');
		
	my $can = Term::ANSICanvas->new($bg->height,$bg->width);

	

	my $x =2;
	my $y =2;
	my $ox = 3;
	my $oy = 3;
		
		
	my $dy = 1;
	my $dx = 1;
	
	my $count =0;
	
	my $a = time;
	#while(++$count) #++$count<350)
	while(++$count<1000)
	{
		
		$can->clear;
		#$can->line([ON_BLUE,RED],$y,$x,$y+($oy * sign($dy)),$x+($ox * sign($dx)),' ');
		
		$can->render_canvas(0,0,$bg);
		
		my $new_x = $x+$dx;
		my $new_y = $y+$dy;
		
		# Something there
		if($can->getchr($new_y,$new_x) ne ' ')
		{
			#$dx *= -1 * int(rand()*2+1);
			#$dy *= -1 * int(rand()*2+1);
			if($can->getchr($new_y,$x-$dx) ne ' ')
			{
				if($can->getchr($y-$dy,$new_x) ne ' ')
				{
					if($can->getchr($y-$dy,$x-$dx) ne ' ')
					{
						if($can->getchr($new_y,$x) ne ' ')
						{
							if($can->getchr($y,$new_x) ne ' ')
							{
								print "Stuck!\n";
							}
							else
							{
								print "[Case1]\n";
								$new_y = $y;
								#$dy = 0;
								$dx = 1 - (rand()*2);
							}
						}
						else
						{
							print "[Case2]\n";
							$new_x = $x;
							#$dx = 0;
							$dx = 1 - (rand()*2);
						}
					}
					else
					{
						print "[Case3]\n";
						$new_y = $y-$dy;
						$new_x = $x-$dx;
						$dx = 1 - (rand()*2);
						$dy = 1 - (rand()*2);
					}
				}
				else
				{
					print "[Case4]\n";
					$new_y = $y-$dy;
					#$dy*=-1;
					$dy = 1 - (rand()*2);# if $dx == 0;
				}
			}
			else
			{
				print "[Case5]\n";
				$new_x = $x-$dx;
				#$dx*=-1;
				$dx = 1 - (rand()*2);# if $dx == 0;
			}
			
		}
		
		$x= $new_x;
		$y= $new_y;
		
		#$can->render_canvas(int($y),int($x),$c2);
		$can->string(int($y),int($x),'<white><onblue>#<clear>');
		
		
		$dx = -1 if $dx==0; #; #1 - int(rand()*2) if $dx == 0;
		$dy = -1 if $dy==0; #1 - int(rand()*2) if $dy == 0;
		#$dx = 1 - int(rand()*2) if $dx==0;
		#$dy = 1 - int(rand()*2) if $dy==0;
		
		
		#$x=$y=2 if $x > $can->width-1 || $y > $can->height-1 || $x<0|| $y<0;
# 		$dx = -1 if($x>$can->width-3);
# 		$dy = -1 if($y>$can->height-3);
# 		$dx = 1 if $x<3;
# 		$dy = 1 if $y<3;
		
		
		print CURSOR_OFF . $can->to_string(0,0,'.') . CURSOR_ON;
		
		
		my $b = time;
		my $d = $b-$a;
		my $fps = $count/$d;
		
		print "\n$y,$x           \n($dx,$dy)              \n(".$can->height.",".$can->width.") [frame $count] [".sprintf("%.02f fps",$fps)."]      \n";
		
		
		#sleep 1/30;
		#sleep 1/100; #1/10;
	}
	
	my $b = time;
	my $d = $b-$a;
	my $fps = $count/$d;
	
	print "\n".REVERSE().'[ Demo3 ]'.CLEAR()." >> [ FPS Test ]\n";
	print "d=$d, fps=$fps\n";
	
