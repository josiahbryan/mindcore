use strict;
package MindCore::Agent;
{
	use MindCore::Agent::Sensor;
	use MindCore::Agent::Output;

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
	
	__PACKAGE__->has_many(sensors => 'MindCore::Agent::Sensor');
	__PACKAGE__->has_many(outputs => 'MindCore::Agent::Output');
	 
	sub outputs_for_node
	{
		my $self = shift;
		my $node = shift;
		return undef if !$node;
		return $self->outputs_by_node_type($node->type);
	}
	
	sub outputs_by_node_type
	{
		my $self = shift;
		my $type = shift;
		$type = $type->name if UNIVERSAL::isa($type, 'MindCore::NodeType');
		my @result = MindCore::Agent::Output->search_outputs_by_node_type($self->id, $type);
		return wantarray ? @result : \@result;	
	}
	
	
	# The execute of procedures-
	# Evaluates which procedures to run ...todo, how?
	# updates episodic memory
	# chooses context to operate on
	# chooses current goals/subgoals to persue
	# queries inputs for input and updates context
	# sends output nodes to output acutatorss
	# updates context as to current inputs and outputs
	
	sub current_context { shift->context }
	
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
		my $self = shift;
		# ### HHACKKKKKK
		if($self->{tick_hook})
		{
			$self->{tick_hook}->($self);
		}
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
