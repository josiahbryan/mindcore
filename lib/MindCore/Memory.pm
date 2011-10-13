use strict;
package MindCore::Memory;
{	
	# Memory is episodic - one unit of memory is one episode
	# Episode is a series of temporally sequental nodes representing a single unit of experience identifiable with a series of conceptual or entity nodes 
	
	use base 'AppCore::DBI';
	
	# ### TODO ### Determine if:
	# - This class even needs a database table -
	# - it seems to me all it will do is manage the insertion and linking of episodic memories, 
	#   which they themselves (Episodic Memories, or EMs), are simply a sequence of nodes linked with temporaily sequential links
	# - Each EM could just be a unique node in the database, linking to the symbolic nodes and linking them together
# 	sub new 
# 	{
# 		my $class = shift;
# 		my $self = bless {
# 		
# 			parent_memory	=> undef,	# ref to $class
# 			name		=> '',		# string
# 			
# 			# .... ?
# 			nodes		=> [], 		# refs to MindCore::Node ...?
# 		}, $class;
# 		
# 		return $self;
# 	} 

	my %InstanceCache;
	sub inst
	{
		my $class = shift;
		my $agent = shift;
		return undef if !$agent || !$agent->id;
		my $id = $agent->id;
		my $inst = $InstanceCache{$id};
		if(!$inst)
		{
			$inst = bless { agent => $agent }, $class;
			$InstanceCache{$id} = $inst;
		}
		return $inst;
	}
	
	sub agent { shift->{agent} }

	sub new_episode
	{
		my $self = shift;
		return MindCore::Memory::Episode->new($self);
	}
};

package MindCore::Memory::Episode;
{
	sub new 
	{
		my $class = shift;
		my $mgr = shift;
		my $self = bless { mgr => $self }, $class;
		
		my $memory_node = MindCore::Node->new(MindCore::MemoryNode);
		MindCore::Link->new($self->context_node, [ $self->agent->node, $memory ], MindCore::MemoryLink);	
		$self->{node} = $memory_node;
		
		return $self;
	}
	
	sub mgr { shift->{mgr} }
	sub agent { shift->mgr->agent }
	sub context { shift->agent->context }
	sub context_node { shift->context->node }
	sub node { shift->{node} }
};

1;
