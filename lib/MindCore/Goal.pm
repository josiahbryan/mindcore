use strict;
package MindCore::Goal;
{
	# One goal represents a distinct "desire" or "Target" to try to achieve
	# Goals can have sub-goals, breaking down or relating to the parent goal
	# goals are made up of nodes that describe the goal....? Todo: refine this line

	
	use base 'AppCore::DBI';
	
	__PACKAGE__->meta({
		class_noun	=> 'MindCore Goal',
		#class_title	=> '',
		
		db		=> 'mindcore',
		table		=> 'goals',
		
		schema	=>
		[
			{	field	=> 'goalid',		@AppCore::DBI::PriKeyAttrs	},
			
			{	field	=> 'agentid',		type	=> 'int', linked => 'MindCore::Agent' },
			{	field	=> 'name',		type	=> 'varchar(255)' },
			
			{	field	=> 'parent_goal',	type	=> 'int', linked => 'MindCore::Goal' },
			# Symbolic node representing this goal
			{	field	=> 'node',		type	=> 'int', linked => 'MindCore::Node' },
			
			{	field	=> 'context',		type	=> 'int', linked => 'MindCore::Context' },
			{	field	=> 'ranking',		type	=> 'int' },
			
			# Nodes linked to this goal "describing" it perhaps? ### TODO Determin if this is relevant
			{	field	=> 'nodes',		type	=> 'text' },
			
			### ?????????????? ### TODO ### Define what this is ########
			#{	field	=> 'quantifier',	type	=> 'text' },
			### ?????????????? ### TODO ### Define what this is ########
			
			{	field	=> 'quantifier',	type	=> 'int', linked => 'MindCore::Procedure' },
			 
			 
		]
	});
	
	sub new 
	{
		my $class = shift;
		my $context = shift;
		my $name = shift;
		my $parent_goal = shift;
		
		my $self = $class->insert({
		
			context		=> $context,
			name		=> $name,
			parent_goal	=> $parent_goal,
			#quantifier	=> undef, 	# ???
			
			node	=> MindCore::Node->find_or_create({
				 	name	=> $name,
				 	type	=> MindCore::GoalNode()
				}),
			
			# Should we ref a specific context here...? eg formalize the context to which this goal is relevant?
			# .... ?
			#nodes		=> [], 		# refs to MindCore::Node ...?
		});
		
		#my $link = 
		MindCore::Link->new($self->node, $parent_goal ? $parent_goal->node : $context->node, MindCore::PartOf());
		
		return $self;
	}
	
	sub find_goal
	{
		my $self = shift;
		my $context = shift;
		my $name = shift;
		my $pg = shift;
		my $g = MindCore::Goal->by_field( context => $context, name => $name );
		#print STDERR __PACKAGE__."->find_goal: name:'$name', g by field id:".($g?$g->id:'undef')."\n";
		if(!$g)
		{
			$g = MindCore::Goal->new( $context, $name, $pg );
			#print STDERR __PACKAGE__."->find_goal: name:'$name', created new goal id:".($g?$g->id:'undef')."\n";
		}
		#MindCore::Link->new($g->node, $g->parent_goal ? $g->parent_goal->node : $g->context->node, MindCore::PartOf());
		return $g;
	}
	
	sub evaluate
	{
		my $self = shift;
		my $context = shift; # ?? Ref to [a] MindCore::Context ...
			
		## ... evaluate context to see if goal achieved ..
		#	... either quantify context ...
		#	... possibly create another subgoal to initiate more sensor input to see if goal met ....
		
		return undef; # true or false....
	}
	
};

1;
