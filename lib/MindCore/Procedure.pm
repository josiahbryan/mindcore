use strict;
package MindCore::Procedure;
{
	use JE; # PP Javascript interpreter
	our $JE = undef;
	
	# Used to introspect methods for JE::bind_class() in _methods(), below
	use Class::Inspector;
			
		
		
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
	
	sub procedure 
	{
		my $class = shift;
		my $agent = shift;
		my $name = shift;
		my $script = shift;
		my $options = shift || {};
		
		my $self = $class->by_field( agentid => $agent, name => $name );
		
		if(!$self)
		{		
			$self = $class->insert({ 
				agentid		=> $agent,
				name		=> $name || 'Proc',
				output_type	=> $options->{output_type} || '',
				input_types	=> $options->{input_types} || '',
			});
			
			# Don't have to update() here due to update() after node()
			$self->name('Proc'.$self->id) if $agent->name eq 'Proc';
				
			# Create the node associated with this procedure
			$self->node(MindCore::Node->find_or_create({
					name	=> $self->name, 
					type	=> MindCore::ProcedureNode(),
				})
			);
			$self->update;
		
			# Associate the agent with this procedure if agent suplied
			if($agent)
			{
				my $link = MindCore::Link->new($self->node, $agent->node, MindCore::PartOf());
				#print STDERR __PACKAGE__."->new_procedure: link between self and parent: ".$link."\n";
			}
		}
		
		if($script && $self->script ne $script)
		{
			$self->script($script);
	
			$self->update if $self->is_changed;
		}
		
		return $self;
	}
	
	
# 	sub new 
# 	{
# 		my $class = shift;
# 		my $name = shift || '';
# 		my $self;
# 		if($name)
# 		{
# 			$self = $class->find_or_create({
# 			
# 				#parent_goal	=> undef,	# ref to $class
# 				name		=> $name,	# string
# 				#input_node_types => [],		# quantiy node types perhaps? or do we need to quantiy links those nodes have as well..?
# 				#output_type	 => 'nodes', 	#nodes or truth value
# 				
# 				#node		=> undef,	# refs our node for this procedure in mindcore
# 				
# 				#script		=> $script,	# block of EMCAScript3 script to execute
# 				
# 			});
# 		}
# 		else
# 		{
# 			$self = $class->insert({ name => '' });
# 		}
# 		
# 		return $self;
# 	}
	
	
	sub _methods 
	{
		my $pkg = shift;
		my @list;
		if(UNIVERSAL::isa($pkg, 'Class::DBI'))
		{
			@list = map { $_.'' } $pkg->columns;
		}
		push @list, @{ Class::Inspector->methods($pkg, 'public') || [] };
		use Data::Dumper;
		#print Dumper $pkg, \@list;
		return \@list;
	}
	
	sub execute
	{
		my $self = shift;
		my $input_nodes = shift;
		my $agent = shift || $self->agentid;
		
		# Setup JE
		if(!$JE)
		{
			$JE = JE->new;
			$JE->new_function( find_node => \&MindCore::_node );
			$JE->new_function( link => \&MindCore::_link );
			$JE->new_function( print => sub { print AppCore::Common::date()," [JE] ", @_, "\n" } );
			
			$JE->bind_class(
				package		=> 'MindCore::Node',
				constructor	=> 'find_node',
				methods		=> _methods('MindCore::Node'),
			);
					
			$JE->bind_class(
				package		=> 'MindCore::Link',
				constructor	=> 'new',
				methods		=> _methods('MindCore::Link'),
			);
			
			$JE->bind_class(
				package		=> 'MindCore::Procedure',
				constructor	=> 'procedure',
				methods		=> _methods('MindCore::Procedure'),
			);
			
			$JE->bind_class(
				package		=> 'MindCore::Context',
				constructor	=> 'new_context',
				methods		=> _methods('MindCore::Context'),
			);
			
			$JE->bind_class(
				package		=> 'MindCore::Goal',
				constructor	=> 'find_goal',
				methods		=> _methods('MindCore::Goal'),
			);
			
			$JE->bind_class(
				package		=> 'MindCore::Link::Destination::Array',
				constructor	=> undef,
				methods		=> _methods('MindCore::Link::Destination::Array'),
			);
			
			
		}
		
		$JE->{proc} = $self;
		$JE->{context} = $agent->current_context;
		$JE->{input_nodes} = $input_nodes;
		$JE->{agent} = $agent;
		
		my $result = $JE->eval($self->script);
		if($@)
		{
			die "Error parsing script:\n\t$@\nScript:\n".$self->script;
		}
		
		$result = $result->value if UNIVERSAL::isa($result, 'JE::Object::Proxy');
		
# 		use Data::Dumper;
# 		print "Result: ".Dumper($result);
		
		return $result;
		
		# execute 'script'
		# acquire output
		# convert to truthvalue or lookup node as needed
		# apply rewards to lti/sti values of nodes used as useful inputs...?
		# return value
	}

};

1;
