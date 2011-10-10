use strict;

package MindCore::NamedUUIDType;
{
	sub NAME() {0};
	sub UUID() {1};	
	sub PARENTS() {2};
	
	our %XREF;
	our %ALL_TYPES_XREF;
	
	use overload
		'""' => sub {
			my $self = shift;
			my $pkg = ref($self);
			#$pkg =~ s/^.*::([^\::]+)$/$1/g;
			return $pkg."(".$self->[NAME].")"; 
		};
	
	sub new {
		my ($class,$name,$uuid,$parent_list) = @_;
		my $self = bless [ $name, $uuid, $parent_list ], $class;
		$XREF{$class.'::'.$uuid} = $self;
		$XREF{$class.'::'.$name} = $self;
		$ALL_TYPES_XREF{$name}   = $self;
		$ALL_TYPES_XREF{$uuid}   = $self;
		#print STDERR "$class: $name -> $uuid\n";
		return $self;
	}
	
	sub inherits 
	{
		my $self = shift;
		my $other_type = shift;
		return 1 if $self->[NAME] eq $other_type->[NAME];
		my @list = @{$self->[PARENTS] || []};
		return 0 if !@list;
		foreach my $parent (@list)
		{
			next if !$parent;
			return 1 if $parent->[NAME] eq $other_type->[NAME];
			return 1 if $parent->inherits($other_type);
		}
		return 0;
	}
	
	sub name { shift->[NAME] }
	sub uuid { shift->[UUID] }
	sub lookup
	{
		my $class = shift;
		my $value = shift;
		if($class eq 'MindCore::NamedUUIDType')
		{
			return $ALL_TYPES_XREF{$value};
		}
		
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
