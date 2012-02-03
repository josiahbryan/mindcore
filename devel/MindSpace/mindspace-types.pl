#!/usr/bin/perl
use strict;
use Data::Dumper;

my $input_file         = shift || 'MindSpace.types';
my $output_protos_node = 'mindspace-types.node.prototypes';
my $output_protos_link = 'mindspace-types.link.prototypes';
my $output_defns       = 'mindspace-types.definitions';

open(FILE,     "<$input_file")         || die "Cannot read $input_file: $!";
open(PROTOS_N, ">$output_protos_node") || die "Cannot write to $output_protos_node: $!";
open(PROTOS_L, ">$output_protos_link") || die "Cannot write to $output_protos_link: $!";
open(DEFNS,    ">$output_defns")       || die "Cannot write to $output_defns: $!";

my $date = `date`;
$date =~ s/[\r\n]//g;

print PROTOS_N "/** \\file $output_protos_node - Generated from $input_file by $0 on $date **/\n\n";
print PROTOS_L "/** \\file $output_protos_link - Generated from $input_file by $0 on $date  **/\n\n";
print DEFNS    "/** \\file $output_defns - Generated from $input_file by $0 on $date  **/\n\n";

$|++;
print "Generating mindspace-types node/link prototypes and definitions.";

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
	if( $line_type =~ /node/i)
	{
		#print STDERR "Node!\n";	
		#print DEFNS qq{DefineNodeType(${name},	"${uuid}");\n};
		print DEFNS "static MNodeType MNodeType_${name} ( \"$name\", \"$uuid\");\n";
		print DEFNS "const MNodeType & MNodeType::${name}() { return MNodeType_${name}; }\n";
	
		print PROTOS_N "/** Shared reference to special node type ${name} */\n",
				"static const MNodeType & ${name}();\n"; 
	}
	else
	{
		#my $macro =  ? 'DefineLinkTypeList' : 'DefineLinkType';
		
		#print DEFNS qq{${macro}(${name},	"${uuid}");\n};
		print DEFNS "static MLinkType MLinkType_${name} ( \"$name\", \"$uuid\"";
		if(lc($line_type) ne 'link')
		{
			my @parents = split /,/, $line_type;
			#my @list = map { "MLinkType::${_}()" } @parents;
			#my $cpp = 'QList<MNodeType>() << '. join(' << ', @list);
			my $cpp = 'QStringList() << '. join(' << ', map {"\"$_\"" } @parents);
			print DEFNS ", true, $cpp"; 
		}
		elsif(lc $list_flag eq 'true' || $list_flag eq '1' || lc $list_flag eq 'list')
		{
			print DEFNS ", true";
		}
		print DEFNS " );\n";
		
		print DEFNS "const MLinkType & MLinkType::${name}() { return MLinkType_${name}; }\n";
		
		print PROTOS_L "/** Shared reference to special link type ${name} */\n",
				"static const MLinkType & ${name}();\n"; 
		
	}
	
	print ".";
}
print "Done.\n";
close(FILE);
close(PROTOS_N);
close(PROTOS_L);
close(DEFNS);
