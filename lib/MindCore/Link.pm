use strict;

# Used as values below
use MindCore::TruthValue;

package MindCore::Link::Destination;
{
	use base 'AppCore::DBI';
	
	__PACKAGE__->meta({
		class_noun	=> 'MindCore Link Destination',
		class_title	=> 'Destination of a MindCore link',
		
		db		=> 'mindcore',
		table		=> 'link_to',
		
		schema	=>
		[
			{
				'field'	=> 'destid',
				'extra'	=> 'auto_increment',
				'type'	=> 'int(11)',
				'key'	=> 'PRI',
				readonly=> 1,
				auto	=> 1,
			},
			{	field	=> 'linkid',	type	=> 'int', linked => 'MindCore::Link' },
			{	field	=> 'nodeid',	type	=> 'int', linked => 'MindCore::Node' },
		]
	
	});
};

package MindCore::Link::Destination::Array;
{
	use overload
		'""' => sub {
			my $self = shift;
			return join(',', map { ref $_ ? $_->id : ''.$_ } @$self);
		};
	sub new
	{
		my $class = shift;
		my $list  = shift;
		my $blessed = bless (ref $list eq 'ARRAY' ? $list : [$list]), $class;
		#use Data::Dumper;
		#print Dumper $blessed;
		return $blessed;
	}
};

package MindCore::Link;
{
	use base 'AppCore::DBI';
	
	__PACKAGE__->meta({
		class_noun	=> 'MindCore Link',
		class_title	=> 'Link between two or more MindCore nodes',
		
		db		=> 'mindcore',
		table		=> 'links',
		
		schema	=>
		[
			{
				'field'	=> 'linkid',
				'extra'	=> 'auto_increment',
				'type'	=> 'int(11)',
				'key'	=> 'PRI',
				readonly=> 1,
				auto	=> 1,
			},
			#{	field	=> 'mindid',	type	=> 'int', linked => 'MindCore' },
			{	field	=> 'node',	type	=> 'int', linked => 'MindCore::Node' },
			{	field	=> 'to_nodes',	type	=> 'text' },
			{	field	=> 'type',	type	=> 'varchar(255)' },
			{	field	=> 'tv',	type	=> 'varchar(255)' },
			{	field	=> 'tv1',	type	=> 'float' },
			{	field	=> 'tv2',	type	=> 'float' },
			
			# Link 'to' values are represented in a secondary table
		]
	});
	
	# Used in MindCore::Node::incoming_links()
	__PACKAGE__->set_sql('incoming_links'         => qq{select L.* from links L, link_to L2 where L2.linkid=L.linkid and L2.nodeid=?});
	__PACKAGE__->set_sql('incoming_links_by_type' => qq{select L.* from links L, link_to L2 where L2.linkid=L.linkid and L2.nodeid=? and L.type=?});
	
	__PACKAGE__->has_a(type => 'MindCore::LinkType',
		inflate	=> sub {
			my $str = shift;
			#print STDERR __PACKAGE__."->inflate(LinkType): '$str'\n";
			return MindCore::LinkType->lookup($str) || eval('MindCore::'.$str);
		},
		deflate => sub {
			my $type = shift;
			return $type->name;
		},
	);
	
	__PACKAGE__->has_a(tv => 'MindCore::TruthValue',
		inflate	=> sub {
			my $str = shift;
			my ($v1,$v2) = split /:/, $str;
			MindCore::TruthValue->new($v1,$v2);
		},
		deflate => sub {
			my $tv = shift; 
			#my $tv = $self->tv;
			return join(':', $tv->value1+0, $tv->value2+0);
		}
	);
		
	__PACKAGE__->has_a(to_nodes => 'MindCore::Link::Destination::Array',
		inflate	=> sub {
			my $str = shift;
			
 			return $str if UNIVERSAL::isa($str, 'MindCore::Link::Destination::Array');
 			my $self = shift;
 			my @list = split /,/, $str; #MindCore::Link::Destination->search( linkid => $self->id );
 			my $obj = MindCore::Link::Destination::Array->new( [ map { MindCore::Node->retrieve($_) } @list ] );
 			return $obj;
		
		},
		deflate => sub { 
			my $obj = shift;
			my $self = shift;
			
			#MindCore::Link::Destination->find_or_create( linkid => $self->id, nodeid => $_->id ) foreach  @{$obj || []};
			return join(',', map { $_->id } @{$obj || []});
		}
	);
	
	# Add a 'has_many' relation to the Link for the destinations (even though we handle it via a to_nodes array) 
	# when a link is deleted, the destination pointers are deleted as well
	__PACKAGE__->has_many(dest_links => 'MindCore::Link::Destination');
	
	
	
	use overload
		'""' => sub {
			my $self = shift;
			#print STDERR __PACKAGE__."->stringify: id:".$self->id.", type:".$self->type."\n";
			return (ref $self->type ? $self->type->name : ($self->type ? $self->type : "Link")).
				"(".$self->node."->".
				 (ref $self->to_nodes && scalar @{$self->to_nodes()} > 1 ? ('['.join(',', map { $_."" } @{$self->to_nodes || []} ).']') : ''.@{$self->to_nodes||[]}[0]).
				"), tv=".$self->tv;
		};
	
	
	sub new
	{
		my $class = shift;
		my ($from,$to,$type,$tv) = @_;
		
		my $self;
		
		#if(my @search $class->by_field(name => $name, type => $type->uuid );
		#use Data::Dumper;
		#print Dumper $to;
		
		$tv ||= MindCore::TruthValue::TRUE;
		
		my $self = $class->find_or_create( {
			node		=> $from->id,
			to_nodes	=> MindCore::Link::Destination::Array->new($to), #ref $to eq 'ARRAY' ? $to : [$to]),
			type		=> $type->name,
			tv		=> $tv,
			tv1		=> $tv->value1,
			tv2		=> $tv->value2,
		} );
		
		if(!$from)
		{
			die "Invalid link: no \$from node specified";
		}
		
		
		MindCore::Link::Destination->find_or_create( linkid => $self->id, nodeid => $_->id ) foreach  @{$self->to_nodes || []};
		
		#$from->{links} ||= [];
		#push @{$from->{links}}, $self;
		
		return $self;
		
	}
};

1;
