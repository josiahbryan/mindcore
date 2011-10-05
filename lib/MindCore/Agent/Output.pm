use strict;

package MindCore::Agent::OutputType;
{
	use base qw/MindCore::NamedUUIDType/;
};

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
			
			{	field	=> 'type',		type	=> 'varchar(255)' },
			{	field	=> 'name',		type	=> 'varchar(255)' },
			{	field	=> 'subclass',		type	=> 'varchar(255)' }, # Perl subclass of MindCore::Agent::Output that implements the required functionality 
			{	field	=> 'node',		type	=> 'int', linked => 'MindCore::Node' }, # Specific Entity Node
			# TODO Enum input node types...? Or links? Or some other pattern?
		]
	});
	
	__PACKAGE__->has_a(type => 'MindCore::Agent::OutputType',
		inflate	=> sub {
			my $str = shift;
			#print STDERR __PACKAGE__."->inflate(LinkType): '$str'\n";
			return MindCore::Agent::OutputType->lookup($str) || eval('MindCore::'.$str);
		},
		deflate => sub {
			my $type = shift;
			return $type->name;
		},
	);
	
};

1;
