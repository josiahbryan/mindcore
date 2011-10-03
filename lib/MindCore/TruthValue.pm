use strict;
package MindCore::TruthValue;
{
	use overload
		'""' => sub {
			my $self = shift;
			return "TruthValue($self->{value1}" . ($self->{value2} ? ",$self->{value2}" : "") .")"; 
		};

	sub new {
		my ($class,$value1,$value2) = @_;
		
		if($value1 =~ /TruthValue\(([^\,\)]+)(?:,([^\)]+))?\)/)
		{
			$value1 = $1+0;
			$value2 = $2+0;
		}
		
		$value1 = 0 if !$value1;
		$value2 = 0 if !$value2;
		
		return bless { value1 => $value1, value2 => $value2 }, $class;
	}
	
	sub value  { shift->{value1} } 
	sub value1 { shift->{value1} }
	sub value2 { shift->{value2} }
	
	my $True = __PACKAGE__->new(1);
	sub TRUE { return $True; }
	
	my $False = __PACKAGE__->new(0);
	sub FALSE { return $False; }
	
	my $Unknown = __PACKAGE__->new(-1,-1);
	sub UNKNOWN { return $Unknown; }
};

1;