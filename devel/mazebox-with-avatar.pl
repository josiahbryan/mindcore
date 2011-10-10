#!/usr/bin/perl
use strict;
use lib '../lib';
use MindCore qw/:namespace :types _node/;
use Data::Dumper;
use Time::HiRes qw/time sleep/;
use Term::ANSICanvas;

use MindCore::Agent::Avatar;
use MindCore::Agent::Sensor;
use MindCore::Agent::Output;

#MindCore->apply_mysql_schema;

package MindCore::Agent::Avatar::Mazebox;
{
	# An 'Avatar' is the connector to the environment for the Agent.
	# Subclasses for specific enviornments should provide the
	# environment-specific code to provide feedback to the agent's ::Sensor
	# and implement whatever ::Output the Agent requests
		
	use base 'MindCore::Agent::Avatar';
	use MindCore qw/:namespace :types _node/;

	sub new 
	{
		my $class = shift;
		my $agent = shift;
		my $can = shift;
		my $self  = $class->SUPER::new($agent);
		$self->{canvas} = $can;
		
		MindCore::Agent::Sensor->find_or_create({ agentid => $agent, type => MindCore::ANSITextSensor }),
		
		return $self;
	}
	
	sub tick
	{
		my $self = shift;
		#my $ctx_node = $self->agent->context->node;
		#my $location_node = $ctx_node->linked_node(MindCore::SimpleXYLocationNode);
		my @sensors = $self->agent->sensors;
		#use Data::Dumper;
		#print Dumper(\@sensors);
		$self->sensor_query($_) foreach @sensors;
		
		$self->agent->tick;
	}
	
	sub canvas { shift->{canvas} }
	
	sub look_at
	{
		my $self = shift;
		my $x = shift;
		my $y = shift;
		my $dx = shift;
		my $dy = shift;
		my $lx = $x+$dx;
		my $ly = $y+$dy;
		my $v = $self->canvas->getchr($ly,$lx);
		#print STDERR __PACKAGE__."->look_at: ($x,$y) + ($dx,$dy) = ($lx,$ly) = ($v)   \n";
		my $char = $v eq ' ' ? undef : $v;
		return $char; #{ char => $char, attr => undef };
	}
	
	sub sensor_query 
	{
		my $self = shift;
		my $sensor = shift;
		#print STDERR "Sensor query: $sensor\n";
		
		return undef if !$sensor;
		if($sensor->type->inherits(MindCore::VisionSensor))
		{
			my $ctx_node = $self->agent->context->node;
			my $location_node = $ctx_node->linked_node(MindCore::SimpleXYLocationNode);
			if(!$location_node)
			{
				$location_node = _node($self->agent->name.' Location', MindCore::SimpleXYLocationNode);
				MindCore::Link->new($ctx_node, [ $self->agent->node, $location_node ], MindCore::LocationOf);	
			}
			
			my $visual_ctx = $ctx_node->linked_node(MindCore::SimpleTextVisualContext);
			if(!$visual_ctx)
			{
				$visual_ctx = _node($self->agent->name.' Visual Context', MindCore::SimpleTextVisualContext);
				MindCore::Link->new( $ctx_node, [ $self->agent->node, $visual_ctx ], MindCore::SpatialLink );
			}
			
			my $ld = $location_node->data;
			my $vd = $visual_ctx->data;
			my ($x,$y) = ($ld->x,$ld->y);
			
			my @points = qw/0,-1 0,1 1,0 -1,0 1,-1 1,1 -1,-1 -1,1/;
			my %visual_data = map { $_ => $self->look_at($x, $y, split ',') } @points;
			use Data::Dumper;
			print Dumper(\%visual_data); 
# 			$vd->update({
# 				#N
# 				'0-1'	=>	$self->lookat(,
# 				#S 
# 				'01'	=>	0,
# 				#E 
# 				'10'	=>	{ char => 'f', attr => 'foxy' },
# 				#W 
# 				'0-1'	=>	0,
# 				#NE 
# 				#EN
# 				'1-1'	=>	0,
# 				#SE
# 				#ES
# 				'11'	=>	0,
# 				#NW
# 				#WN
# 				'-1-1'	=>	0,
# 				#SW
# 				#WS 
# 				'-11'	=>	0,
# 			});
			$vd->update(\%visual_data);
			return $visual_ctx;
		}
		else
		{
			print __PACKAGE__."->sensor_query: Unknown sensor type: ".$sensor->type."\n";
		}
		
		
		return undef; # no input for sensor
	}
	
	sub output_request
	{
		my $self = shift;
		my $output = shift;
		my $output_nodes = shift || [];
		return undef if !$output;
		if($output->type->inherits(MindCore::SimpleXYOutput))
		{
			my @list = @$output_nodes;
			if(!@list)
			{
				print __PACKAGE__."->output_request: No output nodes given to SimpleXYOutput\n";
				return;
			}
			 
			my $output_vector = shift @list;
			if(!$output_vector->type->inherits(MindCore::SimpleXYMovementVector))
			{
				print __PACKAGE__."->output_request: Output node given is not a SimpleXYMovementVector or subtype - it's a ".$output_vector->type."\n";
				return;
			}
			
			my $vd = $output_vector->data;
			my ($vx,$vy) = ($vd->x,$vd->y);
			print "Vector node: $vd [$vx,$vy]\n";
			
			my $ctx_node = $self->agent->context->node;
			
			my $location_node = $ctx_node->linked_node(MindCore::SimpleXYLocationNode);
			if(!$location_node)
			{
				$location_node = _node($self->agent->name.' Location', MindCore::SimpleXYLocationNode);
				MindCore::Link->new($ctx_node, [ $self->agent->node, $location_node ], MindCore::LocationOf);	
			}
			
			my $ld = $location_node->data;
			print "Location node: $ld\n";
			my ($x,$y) = ($ld->x,$ld->y);
			
			my ($new_x, $new_y) = ($vx+$x, $vy+$y);
			my $char = $self->canvas->getchr($new_y,$new_x);
			
			if($char ne ' ')
			{
				print __PACKAGE__."->output_request: Can't move to ($new_x,$new_y) (vec: $vx,$vy) - '$char' is there\n";
			}
			else
			{
				$ld->update({x=>$new_x,y=>$new_y});
				
				# Cheating ... kinda ... not very elegant, anyway.
				#$self->canvas->{agent_pos} = {$x=>$new_x,y=>$new_y};
				$self->{x} = $new_x;
				$self->{y} = $new_y;
				print __PACKAGE__."->output_request: Moved agent to ($new_x,$new_y) (vec: $vx,$vy)\n"; 
			}
			
		}
		else
		{
			print __PACKAGE__."->output_request: Unknown output type: ".$output->type."\n";
		}
		
		return 0; # output not enacted
	}
};

1;

package main;

sub get_avatar
{
	my $canvas = shift;
	
	my $agent = MindCore::Agent->find_agent('Goalie');
	
	my $ctx = $agent->context;
	
	##############################################################################
	#  Find/Create the goal
	##############################################################################
	
# 	my $g = $ctx->find_goal('find red x');
# 	print "G: ".$g->name.", id: ".$g->id."\n";
	# #my @links = $agent->node->incoming_links;
	# my @links = $g->node->links;
	# print "\t",$_,"\n" foreach @links;
	# 
	
	##############################################################################
	#  Seed our path - Find->Search->[Search Algorithms]->[Grid, Rand] (add more later ...)
	##############################################################################
	my $ct = MindCore::ConceptNode;
	Link(_node('find', $ct),   _node('search', $ct), MindCore::DefinedAs);
	
	my $search_node = _node('search algorithm', $ct);
	Link(_node('search', $ct), $search_node, MindCore::ConceptuallyRelatedTo);
	
	my $p_grid = MindCore::Procedure->procedure($agent,'Random Search Pattern','1');
	my $p_rand = MindCore::Procedure->procedure($agent,'Grid Search Pattern','1');
	Link($search_node, $p_grid->node, MindCore::ProcedureImplementationLink);
	Link($search_node, $p_rand->node, MindCore::ProcedureImplementationLink);
	
# 	print "Search Algorithm Links: \n";
# 	my @links = $search_node->links;
# 	print "\t",$_,"\n" foreach @links;
	
	
	##############################
	# Let's pseudo-code a grid-search pattern
	# Basically, move in a straight vecotr (+-1,0 or 0,+-1) until encounter [something]
	# If we still are executing, means that [something] wasn't our goal, so decide
	# on new vector.
	# If vector was (1,0),  change vector to (0,1) for one step, then (-1,0)
	# If vector was (0,1),  change vector to (1,0) for one step, then (0,-1)
	# If vector was (-1,0), change vector to (...) ...
	# ...
	#
	# For this proc, we need:
	# - Last known vector 
	# - Sensor Input (e.g. circle of objects around, ANSITextSensor)
	# 	- for use with "can we move in dir of vector?"
	#
	# Output: SimpleXYMovementVector Node
	#
	# Store in context: Last Known Vector
	#
	my $vec = _node('agent-test-vec',MindCore::SimpleXYMovementVector);
	$vec->set_data({x=>1,y=>0});
	# use Data::Dumper;
	# print Dumper $vec;
	Link( $ctx->node, [ $agent->node, $vec ], MindCore::SpatialLink );
	
	my $visual_ctx = _node('agent-vis-ctx', MindCore::SimpleTextVisualContext);
	$visual_ctx->set_data({
		#N
		'0-1'	=>	0,
		#S 
		'01'	=>	0,
		#E 
		'10'	=>	0, #{ char => 'f', attr => 'foxy' },
		#W 
		'0-1'	=>	0,
		#NE 
		#EN
		'1-1'	=>	0,
		#SE
		#ES
		'11'	=>	0,
		#NW
		#WN
		'-1-1'	=>	0,
		#SW
		#WS 
		'-11'	=>	0,
	});
	Link( $ctx->node, [ $agent->node, $visual_ctx ], MindCore::SpatialLink );
	
	
		
	$p_grid->load_script('pgrid.js');
# 	print "First execution of pgrid:\n";
# 	$p_grid->execute();
# 	print "Next execution of pgrid:\n";
# 	my $output = $p_grid->execute();
# 	print "Output: $output\n";
# 	print "Output data: ".$output->data()."\n";
# 	
# 	die "test done\n";

	my $ctx_node = $agent->context->node;
	my $location_node = $ctx_node->linked_node(MindCore::SimpleXYLocationNode);
	if(!$location_node)
	{
		$location_node = _node($agent->name.' Location', MindCore::SimpleXYLocationNode);
		MindCore::Link->new($ctx_node, [ $agent->node, $location_node ], MindCore::LocationOf);	
	}
	
	$location_node->data->update({x=>2,y=>2});

	# Just a placeholder so we have somewhere to send to now...
	my $output = MindCore::Agent::Output->find_or_create({ agentid => $agent, type => MindCore::SimpleXYOutput, output_node_types=>qw/SimpleXYMovementVector/ }),
	# $agent will call $agent->avatar->output_request($output,[$nodes..]) when it needs an output
	my $avatar = MindCore::Agent::Avatar::Mazebox->new( $agent, $canvas );
	
	$agent->{tick_hook} = sub
	{
		my $node = $p_grid->execute();
		$avatar->output_request($output, [$node]);
	};
	
	return $avatar;

}

use Term::ANSIScreen qw/:cursor :screen/;

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


	my $avatar = get_avatar($can);

	

	my $x =2;
	my $y =2;
# 	my $ox = 3;
# 	my $oy = 3;
		
		
# 	my $dy = 1;
# 	my $dx = 1;
	
	my $count =0;
	
	my $a = time;
	#while(++$count) #++$count<350)
	while(++$count<1000)
	{
		
		print locate(25,0), "Count $count\n";
		$can->clear;
		#$can->line([ON_BLUE,RED],$y,$x,$y+($oy * sign($dy)),$x+($ox * sign($dx)),' ');
		
		$can->render_canvas(0,0,$bg);
		
		$avatar->tick;
		
		
# 		my $new_x = $x+$dx;
# 		my $new_y = $y+$dy;
# 		
# 		# Something there
# 		if($can->getchr($new_y,$new_x) ne ' ')
# 		{
# 			#$dx *= -1 * int(rand()*2+1);
# 			#$dy *= -1 * int(rand()*2+1);
# 			if($can->getchr($new_y,$x-$dx) ne ' ')
# 			{
# 				if($can->getchr($y-$dy,$new_x) ne ' ')
# 				{
# 					if($can->getchr($y-$dy,$x-$dx) ne ' ')
# 					{
# 						if($can->getchr($new_y,$x) ne ' ')
# 						{
# 							if($can->getchr($y,$new_x) ne ' ')
# 							{
# 								print "Stuck!\n";
# 							}
# 							else
# 							{
# 								print "[Case1]\n";
# 								$new_y = $y;
# 								#$dy = 0;
# 								$dx = 1 - (rand()*2);
# 							}
# 						}
# 						else
# 						{
# 							print "[Case2]\n";
# 							$new_x = $x;
# 							#$dx = 0;
# 							$dx = 1 - (rand()*2);
# 						}
# 					}
# 					else
# 					{
# 						print "[Case3]\n";
# 						$new_y = $y-$dy;
# 						$new_x = $x-$dx;
# 						$dx = 1 - (rand()*2);
# 						$dy = 1 - (rand()*2);
# 					}
# 				}
# 				else
# 				{
# 					print "[Case4]\n";
# 					$new_y = $y-$dy;
# 					#$dy*=-1;
# 					$dy = 1 - (rand()*2);# if $dx == 0;
# 				}
# 			}
# 			else
# 			{
# 				print "[Case5]\n";
# 				$new_x = $x-$dx;
# 				#$dx*=-1;
# 				$dx = 1 - (rand()*2);# if $dx == 0;
# 			}
# 			
# 		}
		
		$x = $avatar->{x} || 1; #$can->{agent_pos}->{x} || 1;
		$y = $avatar->{y} || 1; #$can->{agent_pos}->{y} || 1;
		
		#$can->render_canvas(int($y),int($x),$c2);
		$can->string(int($y),int($x),'<white><onblue>#<clear>');
		
		
# 		$dx = -1 if $dx==0; #; #1 - int(rand()*2) if $dx == 0;
# 		$dy = -1 if $dy==0; #1 - int(rand()*2) if $dy == 0;
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
		
		#print "\n$y,$x           \n($dx,$dy)              \n(".$can->height.",".$can->width.") [frame $count] [".sprintf("%.02f fps",$fps)."]      \n";
		print "\n$y,$x           \n(".$can->height.",".$can->width.") [frame $count] [".sprintf("%.02f fps",$fps)."]      \n";
		
		
		#sleep 1;
		#sleep 1/20;
		#sleep 1/30;
		#sleep 1/100; #1/10;
		
	}
	
	my $b = time;
	my $d = $b-$a;
	my $fps = $count/$d;
	
	print "\n".REVERSE().'[ Demo3 ]'.CLEAR()." >> [ FPS Test ]\n";
	print "d=$d, fps=$fps\n";
