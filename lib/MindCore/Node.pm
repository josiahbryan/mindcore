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
			{	field	=> 'extra_data', type	=> 'text' },
		]
	});
	
	# Add a 'has_many' relation to the Node (even though we handle the links() query below) so that
	# when a node is deleted, the links leading from the node are deleted as well
	__PACKAGE__->has_many(child_links => 'MindCore::Link');
	__PACKAGE__->has_many(incoming_link_destinations => 'MindCore::Link::Destination');
	
	# Find outgoing links, optionally by type
	sub links
	{
		my $self = shift;
		my $type = shift;
		if($type)
		{
			#print STDERR "$self: Finding links for type '$type' (ref:".ref($type).")\n";
			#$type = $type->value if UNIVERSAL::isa($type, 'JE::String');
			$type = $type->name  if UNIVERSAL::isa($type, 'MindCore::LinkType');
			my @result = MindCore::Link->search( node => $self, type => $type.'' );
			return wantarray ? @result : \@result
		}
		else
		{
			my @result = MindCore::Link->search( node => $self );
			#print STDERR "__PACKAGE__->links: Returning ".($#result+1)." links\n";
			return wantarray ? @result : \@result
		}
	}
	
	
	# Search links from node (n1) to (x1...xN) nodes by the type of the destination nodes.
	# Call linked_nodes($type) to use this query.
	__PACKAGE__->set_sql('nodes_by_dest_node_type' => qq{select N.* from nodes N, links L, link_to L2 where L.node=? and L.linkid=L2.linkid and L2.nodeid=N.nodeid and N.type=?});
	#__PACKAGE__->set_sql('nodes_by_dest_node_name' => qq{select N.* from nodes N, links L, link_to L2 where L.nodeid=? and L.linkid=L2.linkid and L2.nodeid=N.nodeid and N.name=?});
	 
# 	# This wont work in the SQL
# 	sub links_to
# 	{
# 		my $self = shift;
# 		my $type = shift;
# 		$type = $type->value if UNIVERSAL::isa($type, 'JE::String');
# 		$type = $type->name  if UNIVERSAL::isa($type, 'MindCore::NodeType');
# 		my @result = MindCore::Link->search_link_by_dest_node_type( $self->id, $type );
# 		return wantarray ? @result : \@result;
# 	}
	
	sub linked_nodes
	{
		my $self = shift;
		my $type = shift;
		my $return_first = shift;
		#$type = $type->value if UNIVERSAL::isa($type, 'JE::String');
		$type = $type->name  if UNIVERSAL::isa($type, 'MindCore::NodeType');
		my @result = MindCore::Node->search_nodes_by_dest_node_type( $self->id, $type );
		return shift @result if $return_first;
		return wantarray ? @result : \@result;
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

	# ### \brief Lookup node by name \a $node. 
	# ### \returns a MindCore::Node object if \a $node exists.
	# - If \a $node does not exist AND \a $type is given, this creates a new node object and returns it.
	# - If \a $node does not eixst, and \a $type is undefined or false, returns undef.
	# Note: find_node() caches result of query for faster subsequent lookups.
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
		my $n = $NODE_LOOKUP{$node};
		
		# Update the node type to match $type if its not correct
		#print STDERR "tn[1]: ".$n->type."\n";
		#my $tn = $n->type->name;
		if($n && $type && 
			(!$n->type || 
				($n->type->name ne (ref $type ? $type->name : $type))
			)
		)
		{
			$n->type($type);
			$n->update;
		}
		#print STDERR "tn[2]: ".$n->type." [".$type."]\n";
		return $n;
	}

	sub data#()
	{
		my $self = shift;
		my $dat  = $self->{_user_data_inst} || $self->{_data};
		if(!$dat)
		{
			return $self->{_data} = MindCore::Node::GenericDataClass->_init($self);
		}
		return $dat;
	}
	
	# Method: set_data($ref)
	# If $ref is a hashref, it creates a new ::GenericDataClass wrapper and sets that as the data class for this instance.
	# If $ref is a reference (not a CODE or ARRAY), it checks to see if $ref can get,set,is_changed, and update - if true,
	# it sets $ref as the object to be used as the data class.
	sub set_data#($ref)
	{
		my $self = shift;
		my $ref = shift;
		if(ref $ref eq 'HASH')
		{
			$self->{_data} = MindCore::Node::GenericDataClass->_init($self);
			$self->{_data}->set($_, $ref->{$_}) foreach keys %{$ref || {}};
			$self->{_data}->update;
		}
		elsif(ref $ref && ref $ref !~ /(CODE|ARRAY)/)
		{
			foreach(qw/get set is_changed update/)
			{
				die "Cannot use ".ref($ref)." as a data class for Boards::Post: It does not implement $_()" if ! $ref->can($_);
			}
			
			$self->{_user_data_inst} = $ref;
		}
		else
		{
			die "Cannot use non-hash or non-object value as an argument to Boards::Post->set_data()";
		}
		
		return $ref;
	}

};

# Package: MindCore::Node::GenericDataClass 
# Designed to emulate a very very simple version of Class::DBI's API.
# Provides get/set/is_changed/update. Not to be created directly, 
# rather you should retrieve an instance of this class through the
# MindCore::Node::GenericDataClass->data() method.
# Note: You can use your own Class::DBI-compatible class as a data
# container to be returned by MindCore::Node::GenericDataClass->data(). 
# Just call $post->set_data($my_class_instance).
# Copied from EAS::Workflow::Instance::GenericDataClass.
package JE::Number;
{
	sub TO_JSON {
		my $self = shift;
		return $self->value;
	}
};
package MindCore::Node::GenericDataClass;
{
	use vars qw/$AUTOLOAD/;
	#use Storable qw/freeze thaw/;
	use JSON qw/to_json from_json/;
	use Data::Dumper;
	
	my $json = JSON->new;
	$json->convert_blessed(1); 
			
	
	use overload
		'""' => sub {
			my $self = shift;
			#return ref($self)."($self->{name})";
			return $json->encode($self->{data});
		};
	
	
	sub x
	{
		my($x,$k,$v)=@_;
		#$x->{$k}=$v if defined $v;
		#$x->{$k};
		$x->set($k,$v) if defined $v;
		return $x->get($k);
	}
	
	sub AUTOLOAD 
	{
		my $node = shift;
		my $name = $AUTOLOAD;
		$name =~ s/.*:://;   # strip fully-qualified portion
		
		return if $name eq 'DESTROY';
		
		#print STDERR "DEBUG: AUTOLOAD() [$node] ACCESS $name\n"; # if $debug;
		return $node->x($name,@_);
	}
	


# Method: _init($inst,$ref)
# Private, only to be initiated by the Post instance
	sub _init
	{
		my $class = shift;
		my $inst = shift;
		#print STDERR "Debug: init '".$inst->data_store."'\n";
		my $self = bless {data=>from_json($inst->extra_data ? $inst->extra_data  : '{}'),changed=>0,inst=>$inst}, $class;
		#print STDERR "Debug: ".Dumper($self->{data});
		#delete $self->{data}->{'[object Object]'};
		return $self;
		
	}
	
	sub hash {shift->{data}}

# Method: get($k)
# Return the value for key $k
	sub get#($k)
	{
		my $self = shift;
		my $k = shift;
		return $self->{data}->{$k};
	}

# Method: set($k,$v)
# Set value for $k to $v
	sub set#($k,$v)
	{
		my $self = shift;#shift->{shift()} = shift;
		if(@_ == 1)
		{
			my $item = shift;
			# Compat with JE
			if(UNIVERSAL::isa($item,'JE::Object'))
			{
				$item = $item->value;
			}
			# Allow one to call ->set({key:value,...});
			if(ref($item) eq 'HASH')
			{
				my $hash = shift;
				foreach my $key (keys %$hash)
				{
					$self->set($key,$hash->{$key});
				}
			}
			else
			{
				warn __PACKAGE__."->set($item): Need key->value to set";
			}
			return;
		}
		my ($k,$v) = @_;
		#AppCore::Common::print_stack_trace();
		
		# Special-case for JE compatibility
		if(UNIVERSAL::isa($v,'JE::Number') ||
		   UNIVERSAL::isa($v,'JE::String'))
		{
			$v = $v->value;
		}
		
		$self->{data}->{$k} = $v;
		$self->{changed} = 1;
		return $self->{$k};
	}

# Method: is_changed()
# Returns true if set() has been called
	sub is_changed{shift->{changed}}

# Method: update()
# Commits the changes to the workflow instance object
	sub update
	{
		my $self = shift;
		my $json = $json->encode($self->{data});
		$self->{inst}->extra_data($json);
		$self->{inst}->{extra_data} = $json;
		#print STDERR "Debug: save '".$self->{inst}->extra_data."' on post ".$self->{inst}."\n";
		$self->{inst}->update;
		$self->{changed} = 0;
	}
}

1;
