use strict;
package MindCore::Agent::Avatar;
{
	# An 'Avatar' is the connector to the environment for the Agent.
	# Subclasses for specific enviornments should provide the
	# environment-specific code to provide feedback to the agent's ::Sensor
	# and implement whatever ::Output the Agent requests	


	sub new 
	{
		my $class = shift;
		my $agent = shift;
		my $self  = bless {
			agent => $agent,
		}, $class;
		
		$agent->avatar($self);
		
		return $self;
	}
	
	sub agent { shift->{agent} }
	
	sub sensor_query 
	{
		my $self = shift;
		my $sensor = shift;
		
		return undef; # no input for sensor
	}
	
	sub output_request
	{
		my $self = shift;
		my $output = shift;
		
		return 0; # output not enacted
	}
};

1;
