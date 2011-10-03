use strict;
package MindCore::Agent::Sensor;
{
	
	# Sensor tied to a specific agent or class of agent
	# Takes arbitrary input and converts it to MindCore::Node and links
	# queried by ::Agent
	
	use base 'AppCore::DBI';
	__PACKAGE__->meta({
		class_noun	=> 'MindCore Sensor',
		#class_title	=> '',
		
		db		=> 'mindcore',
		table		=> 'sensors',
		
		schema	=>
		[
			{	field	=> 'sensorid',	@AppCore::DBI::PriKeyAttrs	},
			
			#{	field	=> 'mindid',	type	=> 'int', linked => 'MindCore' },
			{	field	=> 'agentid',		type	=> 'int', linked => 'MindCore::Agent' },
			
			{	field	=> 'name',		type	=> 'varchar(255)' },
			{	field	=> 'subclass',		type	=> 'varchar(255)' }, # Perl subclass of MindCore::Agent::Sensor that implements the required functionality 
			{	field	=> 'node',		type	=> 'int', linked => 'MindCore::Node' }, # Specific Entity Node
			# TODO Enum output node types...? Or links? Or some other pattern?
		]
	});
};

1;