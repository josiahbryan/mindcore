use strict;
package MindCore::Agent;
{
	use base 'AppCore::DBI';
	
	__PACKAGE__->meta({
		class_noun	=> 'MindCore Agent',
		#class_title	=> '',
		
		db		=> 'mindcore',
		table		=> 'agents',
		
		schema	=>
		[
			{	field	=> 'agentid',	@AppCore::DBI::PriKeyAttrs	},
			
			#{	field	=> 'mindid',	type	=> 'int', linked => 'MindCore' },
			{	field	=> 'type',		type	=> 'varchar(255)' }, # TODO: Make DBI object for agent type
			{	field	=> 'name',		type	=> 'varchar(255)' },
			{	field	=> 'notes',		type	=> 'text' },
			{	field	=> 'node',		type	=> 'int', linked => 'MindCore::Node' }, # Specific Entity Node
			{	field	=> 'context',		type	=> 'int', linked => 'MindCore::Context' }, 
			# TODO: Single context? Sub contexts? 
			
		]
	});
	
	# The execute of procedures-
	# Evaluates which procedures to run ...todo, how?
	# updates episodic memory
	# chooses context to operate on
	# chooses current goals/subgoals to persue
	# queries inputs for input and updates context
	# sends output nodes to output acutatorss
	# updates context as to current inputs and outputs
	
	sub new
	{
		my $class = shift;
		my $self = bless {
		
		}, $class;
		
		return $self;
	}
	
	sub find_agent
	{
		my $class = shift;
		my $name = shift;
		
		# Agent names must be unique
		my $self = $class->by_field( name => $name );
		
		# If no agent exists for the given name,
		# Create a new agent
		if(!$self)
		{
#			my $test = MindCore::AgentNode();
# 			use Data::Dumper;
# 			print Dumper $test;
			
			# Note agents automatically get their own node,
			# with the type ::AgentNode, a subtype of ::SpecificEntityNode
			$self = $class->insert({ 
				name => $name,
				node => MindCore::Node->find_or_create({
				 	name	=> $name,
				 	type	=> MindCore::AgentNode()
				})
			});
			
			# Called after insert because ::Context stores our agentid in it's table,
			# and our agentid is not available prior to completion of the insert() call.
			$self->context(MindCore::Context->new_context($self));
			$self->update;
			
			# Node: A MindCore::Link is automatically created by new_context()
			# between the new context's node and our node.
		}
		return $self;
	}
	
	sub tick 
	{
		# ...?
	}
	
	sub avatar
	{
		my $self = shift;
		my $av = shift;
		
		$self->{avatar} = $av if $av;
		
		return $self->{avatar};
	}
	
};

1;
