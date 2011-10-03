use strict;
package MindCore::Agent::Output;
{
		
	# Output takes nodes/links from agent and converts those to
	# arbitrary acutations/actions taken on the enviroment
	# triggered arbitrarily by ::Agent

	use base 'AppCore::DBI';
	
	__PACKAGE__->meta({
		class_noun	=> 'MindCore Output',
		#class_title	=> '',
		
		db		=> 'mindcore',
		table		=> 'outputs',
		
		schema	=>
		[
			{	field	=> 'sensorid',	@AppCore::DBI::PriKeyAttrs	},
			
			#{	field	=> 'mindid',	type	=> 'int', linked => 'MindCore' },
			# Owned by an agent...
			{	field	=> 'agentid',		type	=> 'int', linked => 'MindCore::Agent' },
			
			{	field	=> 'name',		type	=> 'varchar(255)' },
			{	field	=> 'subclass',		type	=> 'varchar(255)' }, # Perl subclass of MindCore::Agent::Output that implements the required functionality 
			{	field	=> 'node',		type	=> 'int', linked => 'MindCore::Node' }, # Specific Entity Node
			# TODO Enum input node types...? Or links? Or some other pattern?
		]
	});
};

1;
