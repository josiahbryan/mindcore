#!/usr/bin/perl
use strict;
use lib '../lib';
use MindCore qw/:namespace :types _node/;
use Data::Dumper;

# Simple enviro maybe...
# 20 qs?
# Try to "find" something...?
# By asking questions about where it is maybe....?

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

my $proc = MindCore::Procedure->procedure($agent,'Test',<<'--end--');
	
	print("Hello, World!");
	print("Proc Name: ", proc.name());
	print("Proc ID:   ", proc.id());
	
	var node = find_node("Socrates");
	//print("node: ", node.name());
	return node;
	
--end--

$proc->execute();

die $@ if $@;

print "Node:\n";
print "\t".$agent->node."\n";
print "Node ID: ".$agent->node->id."\n";
print "Links:\n";
my @links = $agent->node->incoming_links;
# foreach my $link (@links)
# {
# 	print STDERR "\tIncoming Link #".$link->id."\n";
# 	print STDERR "\t\t'$_' = '".$link->get($_)."'\n" foreach $link->columns; 
# }
print "\t",$_,"\n" foreach @links;