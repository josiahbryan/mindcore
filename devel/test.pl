#!/usr/bin/perl
use strict;
use lib '../lib';
use MindCore qw/:namespace :types _node/;
use Data::Dumper;

#MindCore->apply_mysql_schema;

# my $type1 = NodeType('SpecificEntityNode','0x0001');
# my $type2  = NodeType('ConceptNode','0x0002');
# #my $tv = TruthValue(1);
# #sprint Dumper \%MindCore::NamedUUIDType::XREF;
# #print "\$type: $type, tv: $tv\n";
# 
#  my $socrates = _node('Socrates', SpecificEntityNode);
#  my $man      = _node('Man',      MindCore::ConceptNode);
#  my $man2     = _node('Human',    MindCore::ConceptNode);
#  my $link     = Link($socrates, [$man,$man2], MindCore::InheritsFromLink, TruthValue(1));
# # 
#  print "\$node: $man [".$man->id."]\n\$link: $link [".$link->id."]\n";
 

my $link1 = Link(_node('Socrates', SpecificEntityNode),	_node('man',    ConceptNode),	InheritsFromLink);
my $link2 = Link(_node('man',      SpecificEntityNode),	_node('mortal', ConceptNode),	InheritsFromLink);
 
# 
print "\$link1: $link1\n\$link2: $link2\n";

 
#print Dumper $socrates->links;

# Package: ACBot::CommonSense
# Base class to access the common sense databases.
# Two major ones that will probably be supported:
# - ConceptNet 2 (Data Files)
# - Freebase (Data Dumps)
# ConceptNet 3 has a REST API but responsiveness is in the order of seconds per query.



