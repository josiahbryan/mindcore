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
			
			# Nodes linked to this goal "describing" it perhaps? ### TODO Determin if this is relevant
			{	field	=> 'nodes',		type	=> 'text' },
			
			### ?????????????? ### TODO ### Define what this is ########
			{	field	=> 'quantifier',	type	=> 'text' },
			### ?????????????? ### TODO ### Define what this is ########
			 
			 
		]
	});
	
	sub new 
	{
		my $class = shift;
		my $self = bless {
		
			parent_goal	=> undef,	# ref to $class
			name		=> '',		# string
			quantifier	=> undef, 	# ???
			
			# Should we ref a specific context here...? eg formalize the context to which this goal is relevant?
			# .... ?
			nodes		=> [], 		# refs to MindCore::Node ...?
		}, $class;
		
		return $self;
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
