use strict;

package MindCore::Agent::SensorType;
{
	use base qw/MindCore::NamedUUIDType/;
};

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
			# Owned by an agent...
			{	field	=> 'agentid',		type	=> 'int', linked => 'MindCore::Agent' },
			
			{	field	=> 'type',		type	=> 'varchar(255)' },
			{	field	=> 'name',		type	=> 'varchar(255)' },
			{	field	=> 'subclass',		type	=> 'varchar(255)' }, # Perl subclass of MindCore::Agent::Sensor that implements the required functionality 
			{	field	=> 'node',		type	=> 'int', linked => 'MindCore::Node' }, # Specific Entity Node
			# TODO Enum output node types...? Or links? Or some other pattern?
		]
	});
	
	__PACKAGE__->has_a(type => 'MindCore::Agent::SensorType',
		inflate	=> sub {
			my $str = shift;
			#print STDERR __PACKAGE__."->inflate(LinkType): '$str'\n";
			return MindCore::Agent::SensorType->lookup($str) || eval('MindCore::'.$str);
		},
		deflate => sub {
			my $type = shift;
			return $type->name;
		},
	);
};

1;