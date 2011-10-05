use strict;

package MindCore::NamedUUIDType;
{
	our %XREF;
	
	use overload
		'""' => sub {
			my $self = shift;
			my $pkg = ref($self);
			$pkg =~ s/^.*::([^\::]+)$/$1/g;
			return $pkg."($self->{name})"; 
		};
	
	sub new {
		my ($class,$name,$uuid,$parent_list) = @_;
		my $self = bless { name => $name, uuid => $uuid, parent_list => $parent_list }, $class;
		$XREF{$class.'::'.$uuid} = $self;
		$XREF{$class.'::'.$name} = $self;
		#print STDERR "$class: $name -> $uuid\n";
		return $self;
	}
	
	sub inherits {
		my $self = shift;
		my $other_type = shift;
		my @list = @{$self->{parent_list} || []};
		return 0 if !@list;
		foreach my $parent (@list)
		{
			return 1 if $parent->name eq $other_type->name;
			return 1 if $parent->inherits($other_type);
		}
		return 0;
	}
	
	sub name { shift->{name} }
	sub uuid { shift->{uuid} }
	sub lookup
	{
		my $class = shift;
		my $value = shift;
		return $XREF{$class.'::'.$value};
	}
};

package MindCore::NodeType;
{
	use base qw/MindCore::NamedUUIDType/;
};

package MindCore::LinkType;
{
	use base qw/MindCore::NamedUUIDType/;
};


1;
