use strict;
use Data::Dumper;

use lib '/var/www/html/appcore/lib';
use AppCore::Common;

# Load our submodules 
use MindCore::Agent;
use MindCore::Agent::Output;
use MindCore::Agent::Sensor;
use MindCore::Context;
use MindCore::Goal;
use MindCore::Link;
use MindCore::Memory;
use MindCore::NamedUUIDType;
use MindCore::Node;
use MindCore::Procedure;
use MindCore::TruthValue;

$SIG{__DIE__} = sub 
	{
		my $err = join(" ", @_);
		#return if $err =~ /(can't locate|undefined sub|Server returned error: Not permitted for method)/i;
		print STDERR AppCore::Common::print_stack_trace();
		#die "Test";
		
		#my $user = AppCore::Common->context->user;
# 		
		#send_email('josiahbryan@gmail.com','[AppCore Error] '.get_full_url(),"$err\n----------------------------------\n".AppCore::Common::get_stack_trace()."\n----------------------------------\nURL:  ".get_full_url()."\nUser: ".($user ? $user->display : "(no user logged in)\n"),1,$user ? eval '$user->email' || "noemail-empid-$user\@noemail.error" : 'notloggedin@nouser.error' );
# 		
# 		AppCore::Session->save();
		
		#error("Internal Server Error",$err);
		die $err;
		#exit;
	};
	
package MindCore; 
{
	
	
	
	sub apply_mysql_schema
	{
		my $self = shift;
		my @db_objects = qw{
			MindCore::Node
			MindCore::Link
			MindCore::Link::Destination
			MindCore::Agent
			MindCore::Agent::Output
			MindCore::Agent::Sensor
			MindCore::Context
			MindCore::Goal
			MindCore::Procedure
		};
		AppCore::DBI->mysql_schema_update($_) foreach @db_objects;
	}
	
	our %TypeCache;
	our %TypeUUIDs;
	
	sub load_types
	{
		my $input_file = shift || '/opt/mindcore-data/MindCore.types';
		
		open(FILE,"<$input_file") || die "Cannot read $input_file: $!";
		
		no strict 'refs';
			
		while(my $line = <FILE>)
		{
			$line =~ s/[\r\n]//g;
			$line =~ s/\s{2,}/ /g;
			$line =~ s/\/\/.*$//g;
			$line =~ s/(^\s+|\s+$)//g;
			next if !$line;
			my @fields = split /\s/, $line;
			#print Dumper \@fields;
			my ($line_type,$name,$uuid,$list_flag) = @fields;
			
			#print STDERR "$line_type\n";
			
			$TypeUUIDs{$name} = $uuid;
			
			#my $parent_string = join(',',map { 'MindCore::'.$_.'()' } grep { $_ ne 'Node' && $_ ne 'Link'} split( /,/, $line_type ) );
			my @parent_list = map { MindCore::NodeType->lookup($_) } grep { $_ ne 'Node' && $_ ne 'Link'} split( /,/, $line_type );
			
			#print STDERR "Importing type '$name' ... [$parent_string]\n";
			if( $line_type =~ /node/i)
			{
				$MindCore::TypeCache{$name} ||= MindCore::NodeType->new($name,$uuid,[ @parent_list ]);
				
				#print STDERR "Node!\n";
				my $sub = 'sub { '.
					'$MindCore::TypeCache{"'.$name.'"}'.
					# ||= MindCore::NodeType->new("'.$name.'","'.$uuid.'",['.$parent_string.']); '.
					# Next two lines break stuff...
					#'if(@_) { return MindCore::Node->new(@_, $MindCore::TypeCache{"'.$name.'"} } '. 
					#'return $MindCore::TypeCache{"'.$name.'"};'.
				'}';
				
				*{"MindCore::$name"} = eval($sub);
				die "Error importing node type $name:\n\tPerl: $sub\n\tError: $@\n" if @; 
			}
			else
			{
				$MindCore::TypeCache{$name} ||= MindCore::LinkType->new($name,$uuid,[ @parent_list ]);
				#my $sub = 'sub { $MindCore::TypeCache{"'.$name.'"} ||= MindCore::LinkType->new("'.$name.'","'.$uuid.'",['.$parent_string.']); }';
				my $sub = 'sub { $MindCore::TypeCache{"'.$name.'"} }';
				*{"MindCore::$name"} = eval($sub);
				die "Error importing link type $name:\n\tPerl: $sub\n\tError: $@\n" if $@;
				
			}
			
			#print "[$name].";
		}
		close(FILE);
	}
	
	BEGIN 
	{
		load_types();
	};

	sub import 
	{
		no strict 'refs';
	
		my $callpkg = caller();
		my $pkg  = shift;
		my %test = map {$_=>1} @_;
		
		@_ = qw/_node/ if !@_;
		
		foreach my $sub (@_)
		{
			next if $sub =~ /^:/;
			*{"$callpkg\::$_"} = \&{"$pkg\::$_"} foreach @_;
		}
		
		if($test{':types'})
		{
			foreach (keys %TypeUUIDs)
			{
				#print STDERR "Importing $_ to $callpkg\n";
				*{"$callpkg\::$_"} = \&{"$pkg\::$_"};
			}
		}
		
		return unless $test{':namespace'};
		#print STDERR "In MindCore::import(): caller:'$caller',package:'$package',module:'$module'\n";
		
		#*{"main\::TruthValue"} = sub { MindCore::TruthValue->new(@_); };
		my @sub_modules = qw/
			MindCore::NodeType
			MindCore::LinkType
			MindCore::Node
			MindCore::Link
			MindCore::TruthValue
			
			MindCore::Agent
			MindCore::Context
			MindCore::Goal
			MindCore::Memory
			MindCore::NamedUUIDType
			MindCore::Procedure
			MindCore::TruthValue
		/;
		
 		foreach my $sub_module (@sub_modules)
 		{
 			my $sub = $sub_module;
 			$sub =~ s/^${pkg}:://g;
 			my $mod = 'sub { '.$sub_module.'->new( @_ ); }';
 			my $sym = "$callpkg\::$sub";
 			#print STDERR "import: $sub: $mod [$sym]\n";
 			*{"$sym"} = eval($mod); 
 		}
	};

	# Utility method, shortcut to MindCore::Node->_find_node()
	sub _node
	{
		return MindCore::Node->find_node(@_);
	}
};

1;
