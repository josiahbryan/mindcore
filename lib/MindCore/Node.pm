use strict;
package MindCore::Node;
{
	use base 'AppCore::DBI';
	
	__PACKAGE__->meta({
		class_noun	=> 'MindCore Node',
		class_title	=> 'MindCore Node',
		
		db		=> 'mindcore',
		table		=> 'nodes',
		
		schema	=>
		[
			{
				'field'	=> 'nodeid',
				'extra'	=> 'auto_increment',
				'type'	=> 'int(11)',
				'key'	=> 'PRI',
				readonly=> 1,
				auto	=> 1,
			},
			#{	field	=> 'mindid',	type	=> 'int', linked => 'MindCore' },
			{	field	=> 'name',	type	=> 'varchar(255)' },
			{	field	=> 'type',	type	=> 'varchar(255)' },
			{	field	=> 'sti',	type	=> 'float' },
			{	field	=> 'lti',	type	=> 'float' },
		]
	});
	
	# Add a 'has_many' relation to the Node (even though we handle the links() query below) so that
	# when a node is deleted, the links leading from the node are deleted as well
	__PACKAGE__->has_many(child_links => 'MindCore::Link');
	
	# Find outgoing links, optionally by type
	sub links
	{
		my $self = shift;
		my $type = shift;
		if($type)
		{
			#print STDERR "$self: Finding links for type '$type' (ref:".ref($type).")\n";
			return MindCore::Link->search( node => $self, type => $type->name );
		}
		else
		{
			return MindCore::Link->search( node => $self );
		}
	}
	
	# Find incoming links, optionally by type
	sub incoming_links 
	{
		my $self = shift;
		my $type = shift;
		if($type)
		{
			return MindCore::Link->search_incoming_link_by_type( $self->id, $type->name );
		}
		else
		{
			return MindCore::Link->search_incoming_links( $self->id );
		}
	}
	
	__PACKAGE__->has_a(type => 'MindCore::NodeType',
		inflate	=> sub {
			my $str = shift;
			return MindCore::NodeType->lookup($str);
			#eval('MindCore::'.$str);
		},
		deflate => sub {
			my $type = shift;
			return $type->name,
		},
	);
		
	use overload
		'""' => sub {
			my $self = shift;
			#return ref($self)."($self->{name})"; 
			return ($self->type ? $self->type->name : "Node")."(".$self->name.")";
		};
	
	my %NODE_LOOKUP;
	
	sub new 
	{
		my $class = shift;
		my ($name, $type) = @_;
		
		my $self = $class->by_field(name => $name );
		if(!$self)
		{
# 			$self = bless {
# 				name	=> $name,
# 				type	=> $type,
# 				lti	=> 0, # Long Term Importance
# 				sti	=> 0, # Short Term Importance
# 			}, $class;
			$self = $class->insert( {
				name	=> $name,
				type	=> $type->name,
				lti	=> 0, # Long Term Importance
				sti	=> 0, # Short Term Importance
			} );
		}
		
		$NODE_LOOKUP{$name} = $self;
		
		return $self;
	}

# 	sub name { shift->{name} }
# 	sub type { shift->{type} }
# 	sub lti  { shift->{lti}  }
# 	sub lti  { shift->{sti}  }
	
	sub find_node
	{
		my ($class,$node,$type) = @_;
		#print STDERR "[$class] \$class->find_node: node:$node\n";
		if(!$NODE_LOOKUP{$node})
		{
			my $obj = $class->by_field(name => $node);
			#print STDERR "[$class] cache miss, got from db: [".ref($obj)."] obj:'$obj'\n";
			if(!$obj && $type)
			{
				$obj = $class->new($node,$type);
			}
			$NODE_LOOKUP{$node} = $obj;
		}
		#print STDERR "[$class] returning '$NODE_LOOKUP{$node}'\n";
		return $NODE_LOOKUP{$node};
	}
};

1;