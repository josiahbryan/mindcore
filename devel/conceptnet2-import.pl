#!/usr/bin/perl
use strict;
use lib '.';
use MindCore qw/:namespace :types _node/;
use Data::Dumper;

#MindCore->apply_mysql_schema;

# K-Lines: ConceptuallyRelatedTo, ThematicKLine, SuperThematicKLine
# Things: IsA, PartOf, PropertyOf, DefinedAs, MadeOf
# Spatial: LocationOf
# Events: SubeventOf, PrerequisiteEventOf, First-SubeventOf, LastSubeventOf
# Causal: EffectOf, DesirousEffectOf
# Affective: MotivationOf, DesireOf
# Functional: CapableOfReceivingAction, UsedFor
# Agents: CapableOf

my $dbh = AppCore::DBI->dbh('mindcore');
my $sth_check_node = $dbh->prepare('select nodeid from nodes where name=?');
my $sth_ins_node = $dbh->prepare('insert into nodes (name,type,sti,lti) values (?,?,0,0)');
my $sth_check_link = $dbh->prepare('select linkid,tv1,tv2 from links where node=? and to_nodes=? and type=?');
my $sth_update_link = $dbh->prepare('update links set tv=?,tv1=?,tv2=? where linkid=?');
my $sth_ins_link = $dbh->prepare('insert into links(node,to_nodes,type,tv,tv1,tv2) values(?,?,?,?,?,?)');
my $sth_ins_dest = $dbh->prepare('insert into link_to(linkid,nodeid) values (?,?)'); 

sub lastid() { return $dbh->last_insert_id(undef,undef,undef,undef); }
		
			my $source = 'conceptnet';
			my $debug = 1;
			my $data_path = '/opt/acbot/data/';
			#my @files = ('predicates_concise_kline.txt', 'predicates_concise_nonkline.txt', 'predicates_nonconcise_kline.txt', 'predicates_nonconcise_nonkline.txt');
			#my @files = ('predicates_concise_kline.txt', 'predicates_concise_nonkline.txt');
			#my @files = ('predicates_concise_kline.txt', 'predicates_concise_nonkline.txt');
			#my @files = ('predicates_concise_nonkline.txt'); # smallest one for testing
			#my @files = ('predicates_nonconcise_nonkline.txt'); # load next
			#my @files = ('predicates_concise_kline.txt',  'predicates_nonconcise_nonkline.txt', 'predicates_nonconcise_kline.txt');
			my @files = ( 'predicates_nonconcise_nonkline.txt', 'predicates_nonconcise_kline.txt');
			
			my $data_hash = {};
			
				foreach my $file (@files)
				{
					my $abs_path = $data_path . $file;
					
					if(!-f $abs_path)
					{
						warn __PACKAGE__."->prime_cache: $source: $abs_path doesn't exist";
						next;
					}
					
					if(!open(FILE, "<$abs_path"))
					{
						warn __PACKAGE__."->prime_cache: $source: Can't read $abs_path: $!";
						next;
					}
					
					my $line_counter = 0;
					
					print STDERR __PACKAGE__."->prime_cache: $source: Reading $abs_path ...\n" if $debug;
					
					while(my $line = <FILE>)
					{
						$line_counter ++;
						#next if $line_counter < 20482 && $file eq 'predicates_concise_kline.txt';
						#anext if $line_counter < 105644 && $file eq 'predicates_nonconcise_kline.txt';
						next if $line_counter < 269499   && $file eq 'predicates_nonconcise_nonkline.txt';
						my ($linktype, $arg1, $arg2, $freq, $infer) = ($line =~ /^\(([^\s]+)\s+("[^"]*")\s+("[^"]*")\s+"f=(\d+);i=(\d+);"\)/);
						if(!$linktype)
						{
							warn __PACKAGE__."->prime_cache: $source: Error processing $abs_path line $line_counter: $line\n";
							next;
						}
						
						$arg1 =~ s/"//g;
						$arg2 =~ s/"//g;
						
						#my $node = MindCore::Node->find_node( $arg1, MindCore::ConceptNode );
						#$data_hash->{$arg1} = $node;
						
						if(!$data_hash->{$arg1})
						{
							$sth_check_node->execute($arg1);
							if($sth_check_node->rows)
							{
								$data_hash->{$arg1} = $sth_check_node->fetchrow;
							}
							else
							{
								
								$sth_ins_node->execute($arg1, MindCore::ConceptNode->name);
								$data_hash->{$arg1} = lastid();
								print STDERR "[1] Cache miss on node: $arg1, newid:".$data_hash->{$arg1}."\n";
							}
						}
						
						if(!$data_hash->{$arg2})
						{
							$sth_check_node->execute($arg2);
							if($sth_check_node->rows)
							{
								$data_hash->{$arg2} = $sth_check_node->fetchrow;
							}
							else
							{
								
								$sth_ins_node->execute($arg2, MindCore::ConceptNode->name);
								$data_hash->{$arg2} = lastid();
								print STDERR "[2] Cache miss on node: $arg2, newid:".$data_hash->{$arg2}."\n";
							}
						}
						
						#$data_hash->{$arg1} ||= { name => $arg1, links => [] };
						#$data_hash->{$arg2} ||= { name => $arg2, links => [] };
						
# 						push @{$data_hash->{$arg1}->{links}},
# 							{
# 								type	=> $linktype,
# 								linked	=> $arg2,
# 								freq	=> $freq,
# 								infer	=> $infer, 
# 							};
 						my $type = eval('MindCore::'.$linktype);
						die "Unknown link type: $linktype" if !defined $type;
# 						#MindCore::LinkType->lookup($linktype);
# 						#print STDERR "Debug: lookup '$linktype': $type, test2: ".eval('MindCore::'.$linktype)."\n";
# 						my $link = MindCore::Link->new( 
# 							$node, 
# 							MindCore::Node->find_node( $arg2, MindCore::ConceptNode ), 
# 							$type, 
# 							MindCore::TruthValue->new( $freq, $infer )
# 						);

						my @args = ( $data_hash->{$arg1}, $data_hash->{$arg2}, $type->name );
						$sth_check_link->execute( @args );
						if(!$sth_check_link->rows)
						{
							#my $sth_ins_link = $dbh->prepare('insert into links (node,to_nodes,type,tv,tv1,tv2) value s(?,?,?,?,?,?)');
							print STDERR "Cache miss for link $arg1 => $arg2 [arg1id:$data_hash->{$arg1},arg2id:$data_hash->{$arg2},type:".$type->name."] \n";
# 							die Dumper \@args;
# 							die "done";
							
							$sth_ins_link->execute($data_hash->{$arg1}, $data_hash->{$arg2}, $type->name, $freq.':'.$infer,$freq,$infer);
							
							$sth_ins_dest->execute(lastid(), $data_hash->{$arg2});
							
						}
						else
						{
							my $ref = $sth_check_link->fetchrow_hashref;
							if($ref->{tv1} != $freq || $ref->{tv2} != $infer)
							{
								$sth_update_link->execute($freq.':'.$infer,$freq,$infer,$ref->{linkid});
								print STDERR "Fixed link $ref->{linkid}\n";
							}
							
						}
						
						print STDERR "$file:$line_counter: $arg1==>[$linktype]==>$arg2 [arg1id:$data_hash->{$arg1},arg2id:$data_hash->{$arg2}]\n";
						
						
						#eval('MindCore::'.$linktype),
							
						
						#print STDERR __PACKAGE__."->prime_cache: $source: $abs_path:$line_counter: [DEBUG] $arg1 => [$linktype] => $arg2 \t ($freq,$infer)\n";
						
	# 					(CapableOf "red bicycle" "pump" "f=2;i=1;")
	# 					(IsA "spider" "bug" "f=3;i=0;")
	# 					(EffectOf "have party" "tire" "f=2;i=0;")
	# 					(LocationOf "steer wheel" "in car" "f=2;i=0;")
	# 					(LocationOf "waitress" "in restaurant" "f=2;i=0;")
	# 					(IsA "horse" "mammal" "f=17;i=5;")
	# 					(EffectOf "ride bike" "accident" "f=2;i=0;")
					}
					close(FILE);
				}
				
