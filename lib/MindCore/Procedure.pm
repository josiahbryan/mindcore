use strict;
package MindCore::Procedure;
{
	
	# Single procedure stored in a database
	# Tied to a representitve node in MindCore
	# Uses the node in MS to store LTI/STI/VLTI values relevant to the procedure
	# Links to the symbolic procedure node in MS quantify its relevance to other nodes (concepts), etc
	# A procedure is scripted in EMCAScript3 (JavaScript)
	# Goal eventually for the agent to code/change/combine/edit it's own procedures
	# Procedure takes one or more nodes as input
	# Outputs one or more nodes OR a TruthValue
	# Procedure should be able to reference:
	# - Current context (and implicitly reference parents)
	# - Current set of goals (defined by goals set for the context maybe...?)\
	# - Reference the DK (Descriptive Knowledge)- MindCore - store
	# - Add to or remove from Episodic memory ... maybe? 

	
	
	use base 'AppCore::DBI';
	
	__PACKAGE__->meta({
		class_noun	=> 'MindCore Procedure',
		#class_title	=> '',
		
		db		=> 'mindcore',
		table		=> 'procedures',
		
		schema	=>
		[
			{	field	=> 'procid',	@AppCore::DBI::PriKeyAttrs	},
			
			{	field	=> 'agentid',		type	=> 'int', linked => 'MindCore::Agent' },
			{	field	=> 'name',		type	=> 'varchar(255)' },
			{	field	=> 'input_types',	type	=> 'text' },
			{	field	=> 'node',		type	=> 'int', linked => 'MindCore::Node' },
			{	field	=> 'output_type',	type	=> "enum('Node','TruthValue')", null=>0, default=>'Node' },
			{	field	=> 'script',		type	=> 'text' },
		]
	});
	
	
	sub new 
	{
		my $class = shift;
		my $name = shift || '';
		my $self;
		if($name)
		{
			$self = $class->find_or_create({
			
				#parent_goal	=> undef,	# ref to $class
				name		=> $name,	# string
				#input_node_types => [],		# quantiy node types perhaps? or do we need to quantiy links those nodes have as well..?
				#output_type	 => 'nodes', 	#nodes or truth value
				
				#node		=> undef,	# refs our node for this procedure in mindcore
				
				#script		=> $script,	# block of EMCAScript3 script to execute
				
			});
		}
		else
		{
			$self = $class->insert({ name => '' });
		}
		
		return $self;
	}
	
	sub execute
	{
		my $self = shift;
		my $input_nodes = shift;
		
		# execute 'script'
		# acquire output
		# convert to truthvalue or lookup node as needed
		# apply rewards to lti/sti values of nodes used as useful inputs...?
		# return value
	}

};

1;
