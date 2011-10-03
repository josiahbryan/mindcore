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
# 	__PACKAGE__->meta({
# 		class_noun	=> 'MindCore Goal',
# 		#class_title	=> '',
# 		
# 		db		=> 'mindcore',
# 		table		=> 'memory',
# 		
# 		schema	=>
# 		[
# 			{	field	=> 'memoryid',		@AppCore::DBI::PriKeyAttrs	},
# 			
# 			{	field	=> 'agentid',		type	=> 'int', linked => 'MindCore::Agent' },
# 			{	field	=> 'name',		type	=> 'varchar(255)' },
# 			
# 			# Symbolic node representing this agents memory
# 			{	field	=> 'node',		type	=> 'int', linked => 'MindCore::Node' },
# 			
# 		]
# 	});
	
	sub new 
	{
		my $class = shift;
		my $self = bless {
		
			parent_memory	=> undef,	# ref to $class
			name		=> '',		# string
			
			# .... ?
			nodes		=> [], 		# refs to MindCore::Node ...?
		}, $class;
		
		return $self;
	} 

};

1;
