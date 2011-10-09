#!/usr/bin/perl
use strict;
use lib '../lib';
use MindCore qw/:namespace :types _node/;
use Data::Dumper;

#MindCore->apply_mysql_schema;

# Simple enviro maybe...
# 20 qs?
# Try to "find" something...?
# By asking questions about where it is maybe....?

##############################################################################
#  Find the agent
##############################################################################

my $agent = MindCore::Agent->find_agent('Goalie');
# $agent->node->delete;
# $agent->context->node->delete;
# $agent->context->delete;
# $agent->delete;
#exit;
# $agent = MindCore::Agent->find_agent('Goalie');

# print "Ctx Node:\n";
# print "\t".$agent->context->node."\n";
# print "Ctx Links:\n";
# my @links = $agent->context->node->links;
# print "\t",$_,"\n" foreach @links;


# print "Node:\n";
# print "\t".$agent->node."\n";
# print "Node ID: ".$agent->node->id."\n";
# print "Links:\n";
# my @links = $agent->node->incoming_links;
# # foreach my $link (@links)
# # {
# # 	print STDERR "\tIncoming Link #".$link->id."\n";
# # 	print STDERR "\t\t'$_' = '".$link->get($_)."'\n" foreach $link->columns; 
# # }
# print "\t",$_,"\n" foreach @links;

# my $proc = MindCore::Procedure->procedure($agent,'Test',<<'--end--');
# 	
# 	print("Hello, World!");
# 	print("Proc Name: ", proc.name());
# 	print("Proc ID:   ", proc.id());
# 	
# 	var node = find_node("Socrates");
# 	//print("node: ", node.name());
# 	return node;
# 	
# --end--
# 
# $proc->execute();
# 
# die $@ if $@;
# 
# print "Node:\n";
# print "\t".$agent->node."\n";
# print "Node ID: ".$agent->node->id."\n";
# print "Links:\n";
# my @links = $agent->node->incoming_links;
# # foreach my $link (@links)
# # {
# # 	print STDERR "\tIncoming Link #".$link->id."\n";
# # 	print STDERR "\t\t'$_' = '".$link->get($_)."'\n" foreach $link->columns; 
# # }
# print "\t",$_,"\n" foreach @links;

my $ctx = $agent->context;

##############################################################################
#  Find/Create the goal
##############################################################################

my $g = $ctx->find_goal('find red x');
print "G: ".$g->name.", id: ".$g->id."\n";
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

print "Search Algorithm Links: \n";
my @links = $search_node->links;
print "\t",$_,"\n" foreach @links;


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
	'10'	=>	{ char => 'f', attr => 'foxy' },
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

#my @list = $ctx->node()->linked_nodes("SimpleXYMovementVector");
#print "SimpleXYMovementVector in context: ". @list .", first: ".shift(@list)."\n";



##############################################################################
#  Link the goal with its related concepts/entities/predicates
##############################################################################

my $verb = _node('find',  MindCore::ConceptNode);
my $ent  = _node('red x', MindCore::SpecificEntityNode);
Link($g->node, $agent->node, MindCore::PredicateSubjectLink);
Link($g->node, $verb,        MindCore::PredicateLink);
Link($g->node, $ent,         MindCore::PredicateObjectLink);

print "Goal node links:\n";
 #my @links = $agent->node->incoming_links;
my @links = $g->node->links;
print "\t",$_,"\n" foreach @links;

# foreach my $link (@links)
# {
# 	my @dest = @{ $link->to_nodes ||[] };
# 	if(!@dest)
# 	{
	# 		print "\t Link ID ".$link->id.": -- NO DESTINATIONS --\n";
# 		$link->delete;
# 	}
# 	print "\t Link ID ".$link->id.": Dest: [$_]\n" foreach @dest;
# }
# 

#$ent->type(MindCore::SpecificEntityNode);
#$ent->update;
#print "Debug: 'red x' type:".$ent->type."\n";

#$ent->delete;

# Setup test link
Link( $ctx->node, [ $ent, _node("10,24", MindCore::ConceptNode) ], MindCore::LocationOf );

print "Ctx Links:\n";
my @links = $agent->context->node->links;
print "\t",$_,"\n" foreach @links;






$p_grid->load_script('pgrid.js');
print "First execution of pgrid:\n";
$p_grid->execute();
print "Next execution of pgrid:\n";
my $output = $p_grid->execute();
print "Output: $output\n";
print "Output data: ".$output->data()."\n";

die "test done\n";




##############################################################################
#  Create the goal evaluation procedure
##############################################################################

my $proc = MindCore::Procedure->procedure($agent,$g->name.' Evaluation Procedure',<<'--end--');
	
	//var inputs = input_nodes;
	//var node = find_node("Socrates");
	//print("node: ", node.name());
	//return node;
	var g = input_nodes;
	
	print("Input goal: ",g.name());
	
	var pred = g.node().links("PredicateLink");
	var predNode = pred[0].to_nodes().value(0);
	var obj = g.node().links("PredicateObjectLink");
	var objNode = obj[0].to_nodes().value(0);
	
	print("Predicate Concept: ", predNode);
	print("Predicate Object: ", objNode);
	
	var ctx = context;
	print("Context: ", ctx.node());
	
	var found = false;
	var localLink = ctx.node().links("LocationOf");
	print("Found ",localLink.length," 'LocationOf' links");
	for(var i=0;i<localLink.length; i++)
	{
		var link = localLink[i];
		var result = link.dest_by_name(objNode.name());
		if(result.length)
		{
			result = link.dest_by_type("ConceptNode");
			if(result.length)
			{
				print("    Location of "+objNode.name()+" is "+result[0].name());
				found = true;
			}
			else
			{
				print("    LocationOf Link "+i+" hit our object, but did not link to a ConceptNode");
			}
		}
		else
		{
			print("    LocationOf Link "+i+" did not link to our object");
		}
	}
	if(!found)
		print(obj[0].to_nodes().value(0).name()," not found");
	
	
--end--

$proc->execute($g);
