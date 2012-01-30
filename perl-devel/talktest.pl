#!/usr/bin/perl
use strict;
use lib '../lib';
use MindCore qw/:namespace :types _node/;
use Data::Dumper;

MindCore->apply_mysql_schema;

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
#  
#print Dumper $socrates->links;

# Package: ACBot::CommonSense
# Base class to access the common sense databases.
# Two major ones that will probably be supported:
# - ConceptNet 2 (Data Files)
# - Freebase (Data Dumps)
# ConceptNet 3 has a REST API but responsiveness is in the order of seconds per query.

#print "PropertyOf isa ThingLink? ".MindCore::PropertyOf->inherits(MindCore::ThingLink)."\n";

my $acbot = Talk->new;

#AppCore::DBI->db_Main->trace(999);

 print $acbot->respond('what color is the sky?');
 print "\nTest done.\n";
 exit;


while (1) 
{
	print "You: ";
	my $text = <STDIN>;
	chomp($text);
	my $response = $acbot->respond($text);
	print "Computer: $response\n";
}





package Talk;
{

	use Data::Dumper;
	use Lingua::LinkParser::Definitions qw(define);
	use Lingua::LinkParser;
	#use ACBot::CommonSense;
	use MindCore qw/:namespace :types _node/;
	
	use AppCore::Common;
	
	sub new 
	{
		my $class = shift;
		my $opts = shift || {};
		my $self = bless $opts, $class;
		
		$self->{parser} = Lingua::LinkParser->new;
		#$self->{sense}  = ACBot::CommonSense->new;
		
		$self->autocorrect(""); # Prime cache

		return $self;
	}
	
	sub introduction 
	{
		#return "Hi, I'm ACBot! What's your name?";
	}
	
	sub respond 
	{
		my $self = shift;
		my $text = shift;
		my $debug = shift || $self->{debug} || 0;
		
		$text = $self->autocorrect($text);
		
		if($text !~ /\s/ || $text =~ /^query:/i)
		{
			$text =~ s/^query://gi;
			my $object_word = $text;
			my $query = _node(lc $object_word);
			if($query)
			{
				$object_word = guess_title($object_word);
				my @links = $query->links;
				#die Dumper $links[0]->type;
				#return "Found ".scalar(@links)." links from word '$object_word'\n";
				return "Here's what I know about '$object_word':\n". 
					join( "\n", map { " - '$object_word' is ".guess_title($_->type->name)." '".guess_title($_->to_nodes->[0]->name)."'" } @links);
			}
			else
			{
				return "I don't recognize '$object_word' in my database.\n";
			}
			
		}
		
		#print STDERR "[DEBUG] Autocorrected: '$text'\n";
		
		my @struct = @{$self->parse_sentance($text, $debug) || []};
		#print Dumper \@struct;
		my $i;
		
		print "I heard:\n";
		for ($i=1; $i<$#struct; $i++)
		{
			my $data = $struct[$i];
			my $word = $data->{word};
			my $links = $data->{links};
			
			print "\nword $i: ", $word, "\n";

			while (my ($k,$v) = each %{$links} )
				{ print " $k => ", $struct[$v]->{word}, " (", define($k), ")\n"; }
		}
		print "\n";
		
		# Start at 1 to skip the left wall
		for ($i=1; $i<$#struct; $i++)
		{
			my $data = $struct[$i];
			my $word = $data->{word};
			my $links = $data->{links};
			
			# Check if this is a "Question-determiner" word.
			# Ref http://www.link.cs.cmu.edu/link/dict/section-D.html,
			# The third subscript place - D##w - used for Q-Ds
			# Ss*w also appears to indicate Q-Ds
			
# 			print "\nword $i: ", $struct[$i]->{word}, "\n";
# 
# 			while (my ($k,$v) = each %{$struct[$i]->{links}} )
# 				{ print " $k => ", $struct[$v]->{word}, " (", define($k), ")\n"; }

			#print Dumper $links;
			my $is_qd = 0;
			foreach my $key (keys %{$links})
			{
				if($key =~ /^[DS]..w/)
				{
					$is_qd = $links->{$key}; # Remember, key is the ID# of the word in the sentance
				}
			}
			
			if($is_qd)
			{
				# Next find the subj
				my $qd_subj_id = $is_qd;
				my $qd_subj_data = $struct[$qd_subj_id];
				my $qd_subj_links = $qd_subj_data->{links};
				
				#my @link_list = keys %{$qd_subj_links};
				my $subj_verb_id = $qd_subj_links->{Ss};
				if(!$subj_verb_id)
				{
					return "Sorry, you don't make sense - $word $qd_subj_data->{word}?";
				}
				
				# Here, subj_verb should be ID of verb - e.g. is
				my $subj_verb_data = $struct[$subj_verb_id];
				my $subj_verb_links = $subj_verb_data->{links};
				
				my $verb_object_id = undef;
				SUBJ_VERB_OBJECT_SEARCH: foreach my $key (keys %{$subj_verb_links})
				{
					if($key =~ /^O/)
					{
						$verb_object_id = $subj_verb_links->{$key};
						last SUBJ_VERB_OBJECT_SEARCH;
					}
				}
				
				if(!$verb_object_id)
				{
					return "Ummmm, I'm not sure what you meant. $word $qd_subj_data->{word} $subj_verb_data->{word} what?";
				}
				
				my $verb_object_data = $struct[$verb_object_id];
				
				
				# At this point, we have:
				# 	- QD Subject Word (e.g. color),
				#	- Subj Verb (is)
				# 	- Verb Object (sky)
				
				my $object_word = $verb_object_data->{word};
				$object_word =~ s/\..*$//g;
				my $query = _node(lc $object_word);
				return "I don't recognize the word '$object_word', worry!" if !$query;
# 				print "Here's what I know about $object_word:\n". 
# 					join( "\n", map { " - $object_word $_->{type} $_->{linked}" } @{$query || []});
				my $qd_subj_word = $qd_subj_data->{word};
				$qd_subj_word =~ s/\..*$//g;
				
				my $verb_word = $subj_verb_data->{word};
				$verb_word =~ s/\..*$//g;
				
				$|++;
				print "Thinking...";
				my @answer;
				LINK: foreach my $link ($query->links)
				{
					#next if $link->node->id == $query->nodeid;
					TO_NODES: foreach my $to_node (@{ $link->to_nodes || []})
					{
						my $arg2 = $to_node->name; #$link->{linked};
						#print STDERR "Processing link from '$object_word' to '$arg2'\n";
						print ".";
						
						my @link_links = $to_node->links(MindCore::DefinedAs); #$self->{sense}->query($arg2);
# 						if(my $link = shift @link_links)
# 						{
# 							my $to_node = $sublink->to_nodes->[0];
# 							push @answer, $to_node->name;
# 						}
						
	# 					print "Here's what I know about $arg2:\n", 
	# 						join( "\n", map { " - $arg2 $_->{type} $_->{linked}" } @{$link_links || []}), "\n";
						
						SUBLINK: foreach my $sublink (@link_links) #@{$link_links || []})
						{
							#if($sublink->{linked} =~ /$qd_subj_word/)
							NEXT_TO_NODES: foreach my $next_to_node (@{ $sublink->to_nodes || []})
							{
								my $arg3 = $next_to_node->name;
								#print STDERR "Processing link from '$object_word' ".$link->type->name." '$arg2' ".$sublink->type->name." '$arg3'\n";
								#print ".";
								
								if($arg3 eq $qd_subj_word && $sublink->type->name =~ /(Defined)/)
								{
									#print "Debug: Adding '$arg2' because linked '$sublink->{linked}' (type $sublink->{type}) (searched for $qd_subj_word)\n";
									push @answer, $arg2;
									last LINK;
								}
							}
						}
					}
				}
				print "\n";
				if(@answer)
				{
					my %hash = map {guess_title($_) => 1} @answer;
					return guess_title($object_word)." $verb_word probably ".join(' or ', sort keys %hash).".";
				}
				else
				{
					return "I couldn't find a $qd_subj_word for $object_word, sorry!\n";
				}
			}
			else
			{
				return "Sorry, I didn't hear question, not programmed to respond to anything else just yet.";
			}
			
# 			print "\n\tword $i: ", $bigstruct[$i]->{word}, "\n";
# 
# 			while (my ($k,$v) = each %{$bigstruct[$i]->{links}} )
# 				{ print "\t\t $k => ", $bigstruct[$v]->{word}, " \t (", define($k), ")\n"; }
		}
		
		
		return "Thanks for all the fish.";
	}
	
	sub parse_sentance 
	{
		my $self = shift;
		my $text = shift;
		my $debug = shift || $self->{debug} || 0;
		
		my $parser = $self->{parser};
		
		$parser->opts(
				'max_sentence_length' => 70,
				'panic_mode'          => 1,
				'max_parse_time'      => 30,
				'linkage_limit'       => 1000,
				'short_length'        => 10,
				'disjunct_cost'       => 2,
				'min_null_count'      => 0,
				'max_null_count'      => 0,
			);
		
		print "[DEBUG] short: ", $parser->opts('short_length'), "\n" if $debug;
		print "[DEBUG] max_time: ", $parser->opts('max_parse_time'), "\n" if $debug;
		
		my $sentence = $parser->create_sentence($text);
		return undef if !$sentence;
		print "[DEBUG] linkages found: ", $sentence->num_linkages, "\n" if $debug;
		
		if ($sentence->num_linkages == 0) 
		{
			$parser->opts('min_null_count' => 1,
				      'max_null_count' => $sentence->length);
			
			print "[DEBUG] length: ", $sentence->length, "\n" if $debug;
			
			$sentence = $parser->create_sentence($text);
			print "[DEBUG] null linkages found: ", $sentence->num_linkages, "\n" if $debug;
			
			if ($sentence->num_linkages == 0) 
			{
				$parser->opts('disjunct_cost'    => 3,
						'min_null_count'   => 1,
						'max_null_count'   => 30,
						'max_parse_time'   => 60,
						'islands_ok'       => 1,
						'short_length'     => 6,
						'all_short_connectors' => 1,
						'linkage_limit'    => 100
				);
				my $sentence = $parser->create_sentence($text);
				print "[DEBUG] panic linkages found: ", $sentence->num_linkages, "\n" if $debug;
			}
		}
		
# 		for my $i (1 .. $sentence->num_linkages) 
# 		{
			my $i = 1;
			my $linkage = $sentence->linkage($i);
			$linkage->compute_union;
			my $sublinkage = $linkage->sublinkage($linkage->num_sublinkages);
			#print $parser->get_diagram($sublinkage), "\n";
			my @bigstruct = $sentence->get_bigstruct;
# 			print Dumper \@bigstruct; 
# 			for my $i (0 .. $#bigstruct)
# 			{
# 				print "\n\tword $i: ", $bigstruct[$i]->{word}, "\n";
# 	
# 				while (my ($k,$v) = each %{$bigstruct[$i]->{links}} )
# 					{ print "\t\t $k => ", $bigstruct[$v]->{word}, " \t (", define($k), ")\n"; }
# 			}
			#return $parser->get_diagram($sublinkage);
#		}
		
		return \@bigstruct;
	}

	sub autocorrect
	{
		my $self = shift;
		my $text = shift;
		#my $debug = shift || $self->{debug} || 0;
		my @words = split / /, $text;
		
		if(!$self->{autocorrect_hash})
		{
			#print "[DEBUG] Reading autocorrect.txt ...\n";
			if(!open(FILE,"</opt/acbot/data/autocorrect.txt"))
			{
				warn "Unable to read autocorrect.txt: $!";
				return $text;
			}
			my $hash = {};
			while (my $line = <FILE>)
			{
				$line =~ s/[\r\n]//g;
				my ($word,$replace) = $line =~ /^([^\s]+)\s+(.*)$/;
				$hash->{$word} = $replace;
				#print STDERR "[DEBUG] Autocorrect: '$word' => '$replace'\n";
			}
			#print "[DEBUG] Done.\n";
			$self->{autocorrect_hash} = $hash;
		}
		
		my $hash = $self->{autocorrect_hash};
		my @output;
		foreach my $word (@words)
		{
			my $tmp = lc $word;
			my ($tmp2, $punct) = $tmp =~ /([a-zA-Z0-9]+)([\.\?\!:-])?/;
			push @output, defined $hash->{$tmp2} ? $hash->{$tmp2}.$punct : $word;
		}
		return join ' ', @output;
	}
};

1;
