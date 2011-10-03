use strict;
package MindCore::Context;
{
	# Context is useful b/c it should give is ways of introspecting our context:
	# - What goals we're striving for in this context
	# - Relevant SpecificEntities
	# - Temporaly relevance
	# - ...?

	use base 'AppCore::DBI';
	
	__PACKAGE__->meta({
		class_noun	=> 'MindCore Context',
		#class_title	=> '',
		
		db		=> 'mindcore',
		table		=> 'context',
		
		schema	=>
		[
			{	field	=> 'contextid',		@AppCore::DBI::PriKeyAttrs	},
			
			{	field	=> 'agentid',		type	=> 'int', linked => 'MindCore::Agent' },
			{	field	=> 'name',		type	=> 'varchar(255)' },
			
			{	field	=> 'parent_context',	type	=> 'int', linked => 'MindCore::Context' },
			# Symbolic node representing this context
			{	field	=> 'node',		type	=> 'int', linked => 'MindCore::Node' },
			
			# Nodes linked to this context "describing" it perhaps? ### TODO Determin if this is relevant
			{	field	=> 'nodes',		type	=> 'text' },
			
		]
	});
	
	sub new_context 
	{
		my $class = shift;
		my $parent = shift;
		
		my $self;
		my $name;
		
		# If the $parent is an agent ...
		if(UNIVERSAL::isa($parent, 'MindCore::Agent'))
		{
			$self = $class->insert({ agentid => $parent });
			$name = 'Context for Agent "'.$parent->name.'"';
			
			#print STDERR __PACKAGE__."->new_context: Created new contextid ".$self->id."\n";
		}
		# If the parent is another context ...
		elsif(UNIVERSAL::isa($parent, 'MindCore::Context'))
		{
			$self = $class->insert({ 
				agentid        => $parent->agentid,
				parent_context => $parent 
			});
			$name = 'Sub-Context of Context #'. $parent->id;
		}
		# Not supported...
		else
		{
			die __PACKAGE__."->new_context(\$parent): Invalid \$parent argument - must be a MindCore::Agent or MindCore::Context";
		}
		
		# Create the node associated with this context ...
		$self->node(MindCore::Node->find_or_create({
				name	=> $name, 
				type	=> MindCore::ContextNode(),
			})
		);
		$self->update;
		
		# Associate the parent with this context 
		my $link = MindCore::Link->new($self->node, $parent->node, MindCore::PartOf());
		#print STDERR __PACKAGE__."->new_context: link between self and parent: ".$link."\n";
		
		return $self;
	}
	
};

1;
