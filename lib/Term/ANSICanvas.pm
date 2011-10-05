package Term::ANSICanvas;
## @class Term::ANSICanvas
# Implements a textual canvas, with support for clipping, lines, rectangles, "transparency" of sorts, etc.

use Data::Dumper;

#use EAS::Telnet::Common;
use strict;


use vars qw($AUTOLOAD $AUTORESET $EACHLINE @ISA @EXPORT @EXPORT_OK
            %EXPORT_TAGS);

use Exporter ();
@ISA         = qw(Exporter);
@EXPORT      = qw/TTE REVERSE CLEAR CLS LLC LRC LINES ON_MAGENTA CTE VER WRAP_OFF DEL_LINE REGION_UP 
	CYAN ON_WHITE NEXT_LINE VT100 UNDERLINE ASCII NORMAL YELLOW LTE URC SHIFTDN MAGENTA BOLD 
	WHITE ULC ON_YELLOW BELL BTE HOME ON_BLUE REGION_ON ON_GREEN REGION_OFF BLUE CURSOR_OFF 
	HOR DEL_SCREEN BLINK RTE DEL_FROM_BOS CURSOR_ON CURSOR_SAV RED WRAP_ON ON_BLACK GREEN 
	DEL_TO_END SHIFTUP CURSOR_RST DEL_FROM_BEGIN REGION_DOWN INITGRAPHICS ON_RED DEL_TO_EOS 
	HIDDEN RESET DIM INVERSE ON_CYAN BLACK CHR_WIDE CHR_WIDE_TOP CHR_WIDE_BOTTOM CHR_NORMAL 
	LAQUO RAQUO STAR HASH MIDDOT CSI OSC ST BEL trim_ansi_codes rgb2color/;

push @EXPORT, @EAS::Common::EXPORT;

@EXPORT_OK   = @EXPORT;
%EXPORT_TAGS = (all => \@EXPORT);
Exporter::export_ok_tags ('all');


#===============================================================================
#Constants
#===============================================================================

# Useful symbols
sub LAQUO 	{ "\xAB" } 
sub RAQUO 	{ "\xBD" }
sub STAR	{ "\x60" }
sub HASH	{ "\x61" }
sub MIDDOT	{ "\x7E" } 

# Control sequences
sub CSI		{ "\x1B[" }
sub OSC		{ "\x1B]" }
sub ST		{ "\x1B\\" }

#Letters heigh and size
sub CHR_NORMAL      { "\x1B#5"}
sub CHR_WIDE_TOP    { "\x1B#3"}
sub CHR_WIDE_BOTTOM { "\x1B#4"}
sub CHR_WIDE        { "\x1B#6"}

#Screen control
sub VT100      { "\x1B[61\"p"}
sub INITGRAPHICS { "\x1B)0"}
sub LINES      { "\x1B(0"}
sub ASCII      { "\x1B(B"}
sub WRAP_ON    { "\x1B[?7h"}
sub WRAP_OFF   { "\x1B[?7l"}
sub REGION_ON  { "\x1B[?6h"}
sub REGION_OFF { "\x1B[?6l"}

#Deleting
sub DEL_TO_END     { "\x1B[0K"}
sub DEL_FROM_BEGIN { "\x1B[1K"}
sub DEL_LINE       { "\x1B[2K"}
sub DEL_TO_EOS     { "\x1B[0J"}
sub DEL_FROM_BOS   { "\x1B[1J"}
sub DEL_SCREEN     { "\x1B[2J"}
sub CLS            { "\x1B[2J"}

#Cursor control
sub CURSOR_OFF  { "\x1B[?25l" }
sub CURSOR_ON   { "\x1B[?25h" }
sub CURSOR_SAV  { "\x1B7"}
sub CURSOR_RST  { "\x1B8"}
sub REGION_UP   { "\x1BM"}
sub REGION_DOWN { "\x1BD"}
sub NEXT_LINE   { "\x1BE"}
sub HOME        { "\x1B[H"}



our %lines_xlate_pretty = 
(
	q	=> LINES . "q" . ASCII,
	x	=> LINES . "x" . ASCII,
	l	=> LINES . "l" . ASCII,
	k	=> LINES . "k" . ASCII,
	j	=> LINES . "j" . ASCII,
	m	=> LINES . "m" . ASCII,
	t	=> LINES . "t" . ASCII,
	u	=> LINES . "u" . ASCII,
	w	=> LINES . "w" . ASCII,
	v	=> LINES . "v" . ASCII,
	n	=> LINES . "n" . ASCII,
);

our %lines_xlate_text = 
(
	q	=> '-',
	x	=> '|',
	l	=> '+',
	k	=> '+',
	j	=> '+',
	m	=> '+',
	t	=> '|',
	u	=> '|',
	w	=> '-',
	v	=> '-',
	n	=> '+',
);

our %lines_xlate = %lines_xlate_pretty; #text;

#Line drawing
#sub HOR { LINES . "q" . ASCII}
#sub VER { LINES . "x" . ASCII}
#sub ULC { LINES . "l" . ASCII}
#sub URC { LINES . "k" . ASCII}
#sub LRC { LINES . "j" . ASCII}
#sub LLC { LINES . "m" . ASCII}
#sub LTE { LINES . "t" . ASCII}
#sub RTE { LINES . "u" . ASCII}
#sub TTE { LINES . "w" . ASCII}
#sub BTE { LINES . "v" . ASCII}
#sub CTE { LINES . "n" . ASCII}

sub HOR { $lines_xlate{q} }
sub VER { $lines_xlate{x} }
sub ULC { $lines_xlate{l} }
sub URC { $lines_xlate{k} }
sub LRC { $lines_xlate{j} }
sub LLC { $lines_xlate{m} }
sub LTE { $lines_xlate{t} }
sub RTE { $lines_xlate{u} }
sub TTE { $lines_xlate{w} }
sub BTE { $lines_xlate{v} }
sub CTE { $lines_xlate{n} }

#sub SHIFTDN	{ sprintf('%c',15)}

sub BELL	{ sprintf('%c',7)}
sub BEL		{ sprintf('%c',7)}

#Attributes
sub CLEAR     { "\x1B[0m"}
sub NORMAL    { "\x1B[0m"}
sub RESET     { "\x1B[0m"}
sub BOLD      { "\x1B[1m"}
sub DIM       { "\x1B[2m"}
sub UNDERLINE { "\x1B[4m"}
sub BLINK     { "\x1B[5m"}
sub REVERSE   { "\x1B[7m"}
sub INVERSE   { "\x1B[7m"}
sub HIDDEN    { "\x1B[8m"}

#Colors
sub BLACK   { "\x1B[30m"}
sub RED     { "\x1B[31m"}
sub GREEN   { "\x1B[32m"}
sub YELLOW  { "\x1B[33m"}
sub BLUE    { "\x1B[34m"}
sub MAGENTA { "\x1B[35m"}
sub CYAN    { "\x1B[36m"}
sub WHITE   { "\x1B[37m"}

sub ON_BLACK   { "\x1B[40m"}
sub ON_RED     { "\x1B[41m"}
sub ON_GREEN   { "\x1B[42m"}
sub ON_YELLOW  { "\x1B[43m"}
sub ON_BLUE    { "\x1B[44m"}
sub ON_MAGENTA { "\x1B[45m"}
sub ON_CYAN    { "\x1B[46m"}
sub ON_WHITE   { "\x1B[47m"}

my @non_vis = qw/
	ON_WHITE	WHITE
	ON_CYAN 	CYAN
	ON_MAGENTA 	MAGENTA
	ON_BLUE 	BLUE
	ON_YELLOW 	GREEN
	ON_GREEN 	RED
	ON_RED 		BLACK
	HIDDEN INVERSE REVERSE BLINK UNDERLINE DIM BOLD CLEAR BEL BELL
	HOME NEXT_LINE REGION_dOWN REGION_UP CURSOR_RST CURSOR_SAV CURSOR_ON CURSOR_OFF CLS DEL_SCREEN DEL_FROM_BOS DEL_TO_EOS DEL_LINE
	DEL_FROM_BEGIN DEL_TO_END REGION_OFF REGION_ON WRAP_OFF WRAP_ON ASCII LINES INITGRAPHICS VT100 CHR_WIDE CHR_WIDE_BOTTOM CHR_WIDE_TOP
	CHR_NORMAL ST OSC CSI/;
my @vis = qw/CTE BTE TTE RTE LTE LLC RCE URC ULC VER HOR MIDDOT HASH STAR RAQUO LAQUO/;

my @vals1;
push @vals1, eval "$_()" foreach @non_vis;
my @vals2;
push @vals2, eval "$_()" foreach @vis;

undef $@;

s/(\[|\(|\)|\]|\+|\.|\-)/\\$1/g foreach @vals1;
s/(\[|\(|\)|\]|\+|\.|\-)/\\$1/g foreach @vals2;
my $rx1 = '('.join('|',@vals1).')';
my $rx2 = '('.join('|',@vals2).')';


sub trim_ansi_codes
{
	my $str = shift;
	$str =~ s/\x1b\[\d{1,2}(?:;\d{1,2})?\w//g;
	$str=~s/$rx1//gi;
	#$str=~s/$rx2/./gi;
	$str =~ s/33m//g;
	$str;
}

##############################################################################
# Group: Color Conversion

# Variable: %colors
# Stores key=>value pairs where the key is the telnet color name (one of the constants above),
# and the value is the approx rgb value in an array refof that color
my %colors = 
(
	black	=> [qw[0   0   0  ]],
	red	=> [qw[255 0   0  ]],
	green	=> [qw[0   255 0  ]],
	yellow  => [qw[255 255 0  ]],
	blue	=> [qw[0   0   255]],
	magenta => [qw[255 0   255]],
	cyan	=> [qw[0   255 255]],
	white   => [qw[255 255 255]],
);

# Set up the unique value for each color in the list - used for searching the colorspace in <rgb2color>
$colors{$_}->[3] = $colors{$_}->[0]+$colors{$_}->[1]*10+$colors{$_}->[2]*100 foreach keys %colors;
#my @ = map { $colors{$_}->[0]+$colors{$_}->[1]*10+$colors{$_}->[2]*3 => uc ($_) } keys %colors;
#my @sortdist = sort{$a<=>$b}keys %dist;

# Function: rgb2color($rgb_arrayref)
# Find the %color's name, above, that has an rgb pair closest to $rgb
sub rgb2color#($rgb_arrayref)
{
	#print_stack_trace();
	my $rgb = shift;
	return $rgb->[3] if defined $rgb->[3];
	my $sum = $rgb->[0]+$rgb->[1]*10+$rgb->[2]*100;
	my $min = [99999,undef]; #766=255*3+1

	#print STDERR "sum=$sum\n";
	
	my $delta;
	foreach my $color (sort keys %colors)
	{
		$delta = abs($colors{$color}->[3] - $sum);
		#print STDERR "$color: val=$colors{$color}->[3], delta=$delta\n";
		if($delta <= $min->[0])
		{
			$min = [$delta,$color];
		}
	}
	#print STDERR Dumper $rgb,$min->[0],$min->[1];
	
	return uc $min->[1];
}

##############################################################################
# Group: Onto the main module.........


my $DISABLE_ATTRIBUTES = 0;

# Uncomment this block of code to use the CPAN Term::ANSIScreen 
# instead of my company-local EAS::Telnet::Common for ANSI codes.
use Term::ANSIScreen qw/:all/;

sub LINES      { "\x1B(0"}
sub ASCII      { "\x1B(B"}


sub HOR { LINES . "q" . ASCII}
sub VER { LINES . "x" . ASCII}
sub ULC { LINES . "l" . ASCII}
sub URC { LINES . "k" . ASCII}
sub LRC { LINES . "j" . ASCII}
sub LLC { LINES . "m" . ASCII}
sub LTE { LINES . "t" . ASCII}
sub RTE { LINES . "u" . ASCII}
sub TTE { LINES . "w" . ASCII}
sub BTE { LINES . "v" . ASCII}
sub CTE { LINES . "n" . ASCII}
sub CURSOR_OFF  { "\x1B[?25l" }
sub CURSOR_ON   { "\x1B[?25h" }


sub pansi { local $_ = shift; s/\x1B/^/g; $_ }

use Time::HiRes qw/time sleep/;

use strict;

# Store the Attribute fields as constants in an arrayref rather than as a hashref because
# this seems to shave a measurable amount of time off of canvas operations. One benchmark
# on print_demo1 shaved .04sec off - from .20~.19 to .16~.15 on avg (at mark1)
use constant AS_BG    => 0;
use constant AS_FG    => 1;
use constant AS_U     => 2;
use constant AS_BLINK => 3;
use constant AS_BOLD  => 4;
use constant AS__LEN  => 5;

use constant CUR_ROW	=> 0;
use constant CUR_COL	=> 1;

sub new
{
	my $class = shift;
	my $self = bless {}, $class;
	
	my $h = shift || 40;
	my $w = shift || 10;
	
	$self->{width}  = $w;
	$self->{height} = $h;
	
	#$self->{A} 	   = {bg=>undef,fg=>undef,u=>-1,blink=>-1,bold=>-1};
	#$self->{A_all_off} = {bg=>undef,fg=>undef,u=>-1,blink=>-1,bold=>-1};
	$self->{A}		= [undef,undef,undef,undef,undef];
	$self->{A_all_off}	= [undef,undef,undef,undef,undef];
	
	$self->{data} = [];
	$self->{attr} = [];
	
	$self->locate(0,0);
	
	return $self;
}

sub width  { shift->{width}  }
sub height { shift->{height} } 

sub clear
{
	my $x=shift; 
	$x->{data} = [];
	$x->{attr} = [];
}



sub ansi_attrib_split
{
	my $str = shift;
	
	my @list = ($str=~/(
		  	 (?:\x1B\[\d{1,2}m)*		# optional: line drawing chr can have number of colors before it...
			\x1B\(0[qxlkjmtuwvn]\x1B\(B	# line drawing character (1+)
			 (?:\x1B\[\d{1,2}m)*(?:$)?	# optionally followed by a color at the end of the string
			|
			.(?:\x1B\[\d{1,2}m)+$		# match anything followed by any number of colors at the end of the string
			|
			 (?:\x1B\[\d{1,2}m)*.(?:\x1B\[\d{1,2}m)*(?:$)?		# match any number of colors followed by anything
			|
			.				# match anything not matched above
		)/gx);
	
	#if(wantarray) { print called_from().": String:[$str]\n";print "[$_]\n" foreach @list;print "\n-------\n"; }
	return wantarray ? @list : $#list+1;
}


my $regexp_ansi_split = qr/(\x1B\(0.\x1B\(B|.)/o;
sub ansi_split
{
	local $_ = shift;
	
	@_ = /$regexp_ansi_split/g;
	#@_ = /(\x1B\(0.\x1B\(B|.)/g;
	
	#if(wantarray) { print called_from().": String:[$str]\n";print "[$_]\n" foreach @list;print "\n-------\n"; }
	return wantarray ? @_ : $#_+1;
}

my $regexp_ansi_strip = qr/(\x1B\[\d{1,2}m|\x1B\(0[qxlkjmtuwvn]\x1B\(B)/o;
sub ansi_strip
{
	local $_= shift;
	s/$regexp_ansi_strip//g;
	$_;
}

my $regexp_ansi_attrib_strip = qr/\x1B\[\d{1,2}m/o;
sub ansi_attrib_strip
{
	local $_ = shift;
	s/$regexp_ansi_attrib_strip//g;
	$_;
}

sub translate
{
	my $self = shift;
	if(@_ && ref $_[0] eq 'EAS::Telnet::GUI::Rect')
	{
		my $rect = shift;
		$self->{translate} = $rect;
	}
	elsif(@_)
	{
		$self->{translate} = {row=>shift(),col=>shift()};
	}
	$self->{translate};	
}

sub translate_off
{
	undef shift->{translate};
}

sub clip
{
	my $self = shift;
	if(@_ && ref $_[0] eq 'EAS::Telnet::GUI::Rect')
	{
		$self->{clip} = shift;
		$self->{clip}->{row2} = $self->{clip}->{row} + $self->{clip}->{height};
		$self->{clip}->{col2} = $self->{clip}->{col} + $self->{clip}->{width};
	}
	elsif(@_)
	{
		$self->{clip} = {row=>shift(),col=>shift(),height=>shift(),width=>shift()} if @_;
		$self->{clip}->{row2} = $self->{clip}->{row} + $self->{clip}->{height};
		$self->{clip}->{col2} = $self->{clip}->{col} + $self->{clip}->{width};
	}
	$self->{clip};	
}

sub clip_off
{
	undef shift->{clip};
}

sub getchr
{
	my $self = shift;
	my $row  = shift;
	my $col  = shift;
	
	$row = int($row + $self->{translate}->{row});
	$col = int($col + $self->{translate}->{col});
	
	my $clip_r1 =  $self->{clip}->{row}  || 0;
	my $clip_r2 = ($self->{clip}->{row2} || $self->{height}) -1;
	my $clip_c1 =  $self->{clip}->{col}  || 0;
	my $clip_c2 = ($self->{clip}->{col2} || $self->{width}) -1;
	
	return $self if $row < $clip_r1 || $row > $clip_r2;
	return $self if $col < $clip_c1 || $col > $clip_c2;
		
	my $width = $self->{width};

	$self->{data}->[$row]||= [];
	$self->{attr}->[$row]||= [];

	my $chr = $self->{data}->[$row]->[$col];
	
	my $a = $self->{attr}->[$row];
	#$a->[$col] = _atlayer($a->[$col],$self->{A});
	
	return ($chr, $a->[$col]) if wantarray;
	return $chr;
	#return $self;
}

sub putchr
{
	my $self = shift;
	my $row  = shift;
	my $col  = shift;
	my $chr  = shift;
	
	$row = int($row + $self->{translate}->{row});
	$col = int($col + $self->{translate}->{col});
	
	my $clip_r1 =  $self->{clip}->{row}  || 0;
	my $clip_r2 = ($self->{clip}->{row2} || $self->{height}) -1;
	my $clip_c1 =  $self->{clip}->{col}  || 0;
	my $clip_c2 = ($self->{clip}->{col2} || $self->{width}) -1;
	
	return $self if $row < $clip_r1 || $row > $clip_r2;
	return $self if $col < $clip_c1 || $col > $clip_c2;
		
	my $width = $self->{width};

	$self->{data}->[$row]||= [];
	$self->{attr}->[$row]||= [];

	$self->{data}->[$row]->[$col] = $chr;
	
	
	my $a = $self->{attr}->[$row];
	$a->[$col] = _atlayer($a->[$col],$self->{A});
	
		
	return $self;


}

sub string
{
	my $self = shift;
	my $row  = shift;
	my $col  = shift;
	my $str  = shift;
	
	$row = int($row);
	$col = int($col);
	$row = 0 if $row<0;
	$col = 0 if $col<0;
	
	my @tmp;
	if($str=~/<[^\>]+>/)
	{
		my $a;
		#my @line = split /(<[^\>]+>|.*)/, $str;
		my @line = $str=~/(<[^\>]+>|.)/g;	
		$self->locate($row,$col);
		#print Dumper \@line;
		foreach(@line)
		{
			#print "\$_=[$_]\n";
			if(/<(.*)>/)
			{
				#next;
				$a = lc $1;
				#print "a=[$a] <>\n";
				
				#$a=~s/[^\w]//g;
				if($a eq 'f' || $a eq 'off' || $a eq 'clear')
				{
					#print "   --clear--\n";
					$self->attroff();
				}
				else
				{
					$self->attron(
						$a eq 'k' || $a eq 'black'	? BLACK	:
						$a eq 'r' || $a eq 'red'	? RED	:
						$a eq 'g' || $a eq 'green'	? GREEN	:
						$a eq 'y' || $a eq 'yellow'	? YELLOW :
						$a eq 'b' || $a eq 'blue'	? BLUE	:
						$a eq 'm' || $a eq 'magenta'	? MAGENTA :
						$a eq 'p' || $a eq 'purple'	? MAGENTA :
						$a eq 'c' || $a eq 'cyan'	? CYAN	: 
						$a eq 'w' || $a eq 'white'	? WHITE :
						
						$a eq 'ok' || $a eq 'onblack'	|| $a eq 'bgblack'	? ON_BLACK	:
						$a eq 'or' || $a eq 'onred'	|| $a eq 'bgred'	? ON_RED	:
						$a eq 'og' || $a eq 'ongreen'	|| $a eq 'bggreen'	? ON_GREEN	:
						$a eq 'oy' || $a eq 'onyellow'	|| $a eq 'bgyellow'	? ON_YELLOW 	:
						$a eq 'ob' || $a eq 'onblue'	|| $a eq 'bgblue'	? ON_BLUE	:
						$a eq 'om' || $a eq 'onmagenta'	|| $a eq 'bgmagenta'	? ON_MAGENTA 	:
						$a eq 'op' || $a eq 'onpurple'	|| $a eq 'bgpurple'	? ON_MAGENTA 	:
						$a eq 'oc' || $a eq 'oncyan'	|| $a eq 'bgcyan'	? ON_CYAN	: 
						$a eq 'ow' || $a eq 'onwhite'	|| $a eq 'bgwhite'	? ON_WHITE 	:
						
						$a eq 'u' || $a eq 'underline'	? UNDERLINE	:
						$a eq 'l' || $a eq 'bold'	? BOLD		:
						$a eq 'd' || $a eq 'dim' || $a eq 'dark'	? DARK		:
						$a eq 'i' || $a eq 'blink'	? BLINK		:
						
						$a eq 'e' || $a eq 'clear'	? CLEAR		:
						
						''
					);
				}
			}
			else
			{
				#print "_=[$_]\n";
				$self->print($_);
			}
		}
		
		$self->attroff();
		return $self;
	}
	
	
	#my (@chr,@attrs) = ansi_parse $str;
	#print called_from().": string=[$str]\n";
	my @chr = ansi_split ansi_attrib_strip $str; #fsplit//, ansi_strip $str;
	
	$row = int($row + $self->{translate}->{row});
	$col = int($col + $self->{translate}->{col});
	
	my $clip_r1 = $self->{clip}->{row}  || 0;
	my $clip_r2 = ($self->{clip}->{row2} || $self->{height}) -1;
	my $clip_c1 = $self->{clip}->{col}  || 0;
	my $clip_c2 = ($self->{clip}->{col2} || $self->{width}) -1;
	
	return $self if $row < $clip_r1 || $row > $clip_r2;
	

	#return $self if $row > ($self->{clip}->{row2} ? $self->{clip}->{row2} : $self->{height});
	
	#my $width = $self->{width};
	#my $clip_right = $self->{clip}->{col2} || $self->{width};
	
	$self->{data}->[$row]||= [];
	$self->{attr}->[$row]||= [];
	
	
	my $r = $self->{data}->[$row];
	my $a = $self->{attr}->[$row];
	my $s = $self->{A};
	my ($x,$z,@tmp);
	foreach $x (0..$#chr)
	{
		$z = $x+$col;
		#last if $z > $width;
		next if $z < $clip_c1;
		last if $z > $clip_c2;
		
		# This lovely little oneliner strips any attribute codes out of the string (replacing them with ''),
		# and stores them in @tmp. Since 'if' will evaluate @tmp to the scalar number of elements, if @tmp has > 0
		# elements, it executes the call to attron, which 'turns on' each ansi code in @tmp, returning the 
		# resutling hashref {A} to $s.
		# ... 
		# And if no ansi codes are found in the string, @tmp will be 0 length, and $s will use the original {A}
		# hashref from the start of the loop
		#$s = $self->attron(@tmp) if @tmp = ($str=~s/(\x1B\[\d+m)//g);
		
		$r->[$z] = $chr[$x];
		$a->[$z] = _atlayer($a->[$z],$s);
	}
	
	return $self;
	
}

sub ansi_string
{
	my $self = shift;
	my $row  = shift;
	my $col  = shift;
	my $str  = shift;
	
	$row = int($row);
	$col = int($col);
	$row = 0 if $row<0;
	$col = 0 if $col<0;
	
	my @chr = ansi_attrib_split $str; #fsplit//, ansi_strip $str;
	
	$row = int($row + $self->{translate}->{row});
	$col = int($col + $self->{translate}->{col});
	
	my $clip_r1 = $self->{clip}->{row}  || 0;
	my $clip_r2 = ($self->{clip}->{row2} || $self->{height}) -1;
	my $clip_c1 = $self->{clip}->{col}  || 0;
	my $clip_c2 = ($self->{clip}->{col2} || $self->{width}) -1;
	
	return $self if $row < $clip_r1 || $row > $clip_r2;
	
	$self->{data}->[$row]||= [];
	$self->{attr}->[$row]||= [];
	
	
	my $r = $self->{data}->[$row];
	my $a = $self->{attr}->[$row];
	my $s = $self->{A};
	my ($x,$z,@tmp);
	foreach $x (0..$#chr)
	{
		$#tmp = 0;
		
		$z = $x+$col;
		#last if $z > $width;
		next if $z < $clip_c1;
		last if $z > $clip_c2;
		
		# This lovely little oneliner strips any attribute codes out of the string (replacing them with ''),
		# and stores them in @tmp. Since 'if' will evaluate @tmp to the scalar number of elements, if @tmp has > 0
		# elements, it executes the call to attron, which 'turns on' each ansi code in @tmp, returning the 
		# resutling hashref {A} to $s.
		# ... 
		# And if no ansi codes are found in the string, @tmp will be 0 length, and $s will use the original {A}
		# hashref from the start of the loop
		print "x=$x\n"; #,chr[$x]=[$chr[$x]]\n";
		@tmp = $chr[$x]=~/(\x1B\[\d+m)/g;
		if(@tmp)
		{
			$chr[$x]=~s/(\x1B\[\d+m)//g;
			print "ansi=".pansi($_)."\n" foreach @tmp;
			$s = $self->attron(@tmp);
			#print Dumper \@tmp;
		}
		
		
		$r->[$z] = $chr[$x];
		$a->[$z] = _atlayer($a->[$z],$s);
		
		$self->attroff(@tmp) if @tmp;
	}
	
	return $self;
	
}

sub getarg{}


#use Inline C => Config =>
#     BUILD_NOISY => 1; # to make sure we get to
                       # see compiler warnings
                       
#sub _atlayer { _catl($_[0] || [undef,undef,undef,undef,undef],$_[1] || [undef,undef,undef,undef,undef]) }
#sub _atlayer { my $r = _catl($_[0] || [undef,undef,undef,undef,undef],$_[1] || [undef,undef,undef,undef,undef]); die Dumper $r }
#sub _depreciated_atlayer
sub _atlayer
{
	# It is faster to use the ref directly 5 times below, rather than de-refing it once at the top
	# on the order of 500ms over 500,000 iterations
	my $dr  = $_[0];
	# Deref because we want to clone and not just modify original ref
	my @dr2 = @{$_[1] || [undef,undef,undef,undef,undef] };
	
	
	$dr2[AS_BG]	||= $dr->[AS_BG];#	if !defined $dr2[AS_BG];#    || $dr2[AS_BG]    < 0;
	$dr2[AS_FG]	||= $dr->[AS_FG];#	if !defined $dr2[AS_FG];#    || $dr2[AS_FG]    < 0;
	$dr2[AS_U]	||= $dr->[AS_U];#	if !defined $dr2[AS_U];#     || $dr2[AS_U]     < 0;
	$dr2[AS_BOLD]	||= $dr->[AS_BOLD];#	if !defined $dr2[AS_BOLD];#  || $dr2[AS_BOLD]  < 0;
	$dr2[AS_BLINK]	||= $dr->[AS_BLINK]; #	if !defined $dr2[AS_BLINK];# || $dr2[AS_BLINK] < 0;
	
	
	return \@dr2;
}
=head1
my @new = (1,undef,3,undef,5);
my @old = (9,7,9,7,9);

my $r1 = \@new;
my $r2 = \@old;
#die Dumper _atlayer($r2,$r1),$r2,$r1;

use Time::HiRes;

my $cnt = 500000;
my $a = time;
for(0..$cnt){_atlayer($r2,$r1)}
my $b = time;
my $d = $b - $a;
my $avg = $d / $cnt;

print "d = $d, avg=$avg\n";

exit;
=cut

=head1
use Inline C => <<'END_OF_C_CODE';

#define _AS__LEN 4

AV * _catl(SV *sv_old, SV *sv_new)
{
	AV *old; 
	AV *new;
	
	//printf("sv_old=%p,sv_new=%p\n",sv_old,sv_new);
	if(sv_old == NULL)
	{ /* create new array, fill with undef */
		old = newAV();
	}
	else
	{ /* cast ref sv_old to an array */
		old = (AV*)SvRV(sv_old);
		
		if(old == NULL)
		{
			//croak("old is null");
			old = newAV();
		}
	}
	
	if(sv_new == NULL)
	{
		new = newAV();
	}
	else
	{
		new = (AV*)SvRV(sv_new);
		if(new == NULL)
		{
			new = newAV();
		}
	}
	
	SV ** old_tmp, ** new_tmp;
	
	
	av_fill(new,_AS__LEN);
	av_fill(old,_AS__LEN);
	
	int i;
	int t;
	
	AV *newt = newAV();
	for(i=0;i<_AS__LEN;i++)
	{
		new_tmp = av_fetch(new, i, 0);
		if(new_tmp != NULL)
		{
			t = (int)SvIV(*new_tmp);
			//printf("%d = %d\n",i,t);
			new_tmp = newSViv(t);
			SvREFCNT_inc(new_tmp);
			if(av_store(new, i, new_tmp) == NULL)
			{
				SvREFCNT_dec(new_tmp);
			}
		}
		else
		{
			//printf("%d is null\n",i);
		}
	}
	
	new = newt;
	
	//printf("av new=%p, av old=%p\n",new,old);
	
	
	
	for(i=0;i<_AS__LEN;i++)
	{
		/* According to perlapi, caller is responsible for incrmenting ref count of val before call and deincrement if function returns null
		   To inc: SvREFCNT_inc
		   To de-inc: SvREFCNT_dec */
		/* if new[i] is null, set new[i] to old[i] */
		
		new_tmp = av_fetch(new, i, 0);
		
		if(new_tmp != NULL)
		{
		
			if( sv_eq( * new_tmp , &PL_sv_undef) )
			{
				old_tmp = av_fetch(old, i, 0);
				if(old_tmp != NULL)
				{
					t = (int)SvIV(*old_tmp);
					old_tmp = newSViv(t);
					SvREFCNT_inc(old_tmp);
					if(av_store(new, i, old_tmp) == NULL)
					{
						SvREFCNT_dec(old_tmp);
					}
				}
				
			}
		}
		/*
		else
		{
			//croak("null");
			old_tmp = &PL_sv_undef;
			SvREFCNT_inc(*old_tmp);
			if(av_store(new, i, *old_tmp) == NULL)
			{
				SvREFCNT_dec(*old_tmp);
			}
		}
		*/
		
	}
	
	return new;
	
	
}

void add_one(AV * arri) {
int leni, lenj, i, j, t;
AV * arrj;
SV ** elemi, **elemj;

leni = av_len(arri) + 1;
for(i = 0; i < leni; ++i) {
    elemi = av_fetch(arri, i, 0);
    if(elemi == NULL) croak("Got a NULL in arri");
    arrj = (AV*)SvRV(*elemi);
    lenj = av_len(arrj) + 1;

    for(j = 0; j < lenj; ++j) {
       elemj = av_fetch(arrj, j, 0);
       if(elemj != NULL) {
         t = (int)SvIV(*elemj);
	t++;
	sv_setiv(*elemj, t);
         }
       else croak("Got a NULL in arrj");
       }
    }

}

void get_arr(SV* arr) {
int i, leni;
AV * arref;
SV ** elem;

arref = (AV *)SvRV(arr);
leni = av_len(arref) + 1;
for(i = 0; i < leni; ++i) {
    elem = av_fetch(arref, i, 0);
    if(elem != NULL) printf("%d ", (int)SvIV(*elem));
    else croak("Got a NULL in get_arr()");
    }
printf("\n");
}


END_OF_C_CODE
=cut

=head1


use Benchmark ':all';
print "\n";
print join ',', @{ _atlayer(\@old,\@new) };
print "\n";

my $old = \@old;
my $new = \@new;

use Time::HiRes;

my $cnt = 1000000;
my $a = time;
for(0..$cnt){_catl($old,$new)}
my $b = time;
my $d = $b - $a;
my $avg = $d / $cnt;

print "d = $d, avg=$avg\n";


my $a = time;
for(0..$cnt){_atlayer($old,$new)}
my $b = time;
my $d = $b - $a;
my $avg = $d / $cnt;

print "d = $d, avg=$avg\n";

=cut
#cmpthese( 10, { a => '_atlayer(\@old,\@new)', b => '_catl(\@old,\@new)' } ) ;


#for(0..10)
#{
	#print_demo1();
#}



sub _ateq
{
	my $a = shift;
	my $b = shift;
	#print "\t_ateq([$a],[$b])\n";
	return	$a->[AS_BG]	== $b->[AS_BG] &&
		$a->[AS_FG]	== $b->[AS_FG] &&
		$a->[AS_U]	== $b->[AS_U]  &&
		$a->[AS_BOLD]	== $b->[AS_BOLD] &&
		$a->[AS_BLINK]	== $b->[AS_BLINK]
		?1:0;
}

sub _atdiffstr
{
	my $a = shift;
	my $b = shift;
	@_ = ();
	# Underline and blink have no reset codes, therefore compose entire attr block with clear at start
	return _attr($b) if $a->[AS_U] != $b->[AS_U] || $a->[AS_BLINK] != $b->[AS_BLINK];
	
	push @_ , $b->[AS_BG] if $a->[AS_BG] != $b->[AS_BG];
	push @_ , $b->[AS_FG] if $a->[AS_FG] != $b->[AS_FG];
	push @_ , $b->[AS_BOLD] ? BOLD : DARK if $a->[AS_BOLD] != $b->[AS_BOLD];
	
	return join '' , @_;

}

sub dump
{
	print shift->to_string(undef,undef,'.');
}


sub x{join '',@_}


sub iloop($$$$$$$)
{
	my ($line,$arow,$offset_col,$col2,$sw,$lat,$blank) = @_;
	my ($atr,$chr,$col,$cur_attribs);
	my @a = ('') x $sw;
	#print STDERR "$#a=$sw\n";
	
	#print STDERR Dumper $arow;
	
	#$lat = $lat->[AS_BG] + $lat->[AS_FG] + $lat->[AS_U] + $lat->[AS_BOLD] + $lat->[AS_BLINK];
	$lat = join '', $lat->[AS_BG] , $lat->[AS_FG] , $lat->[AS_U] , $lat->[AS_BOLD] , $lat->[AS_BLINK];
	my $x = 0;
	for $col ($offset_col..$col2)
	{
		$chr = $line->[$col];
		$atr = $arow->[$col];

		#print Dumper $atr;
		$cur_attribs = join '', $atr->[AS_BG] , $atr->[AS_FG] , $atr->[AS_U] , $atr->[AS_BOLD] , $atr->[AS_BLINK];
		
		#print STDERR "col=$col, cur_attribs=$cur_attribs, lat=$lat\n";
		
		if($cur_attribs ne $lat)
		{
			$a[$x++] = _attr($atr).(defined $chr ? $chr : $blank);
		}
		else
		{
			$a[$x++] = defined $chr ? $chr : $blank; 
		}

		$lat = $cur_attribs;

	}
	return join '', @a;
}


sub to_string
{
	my $self = shift;
	
	my $dr = shift;
	my $dc = shift;
	
	my $blank = shift || ' ';
	
	my $offset_row = shift || 0;
	my $offset_col = shift || 0;
	my $offset_height = shift || $self->{height} - 1;
	my $offset_width  = shift || $self->{width}  - 1;
	
	$offset_row = 0 if $offset_row < 0;
	$offset_col = 0 if $offset_col < 0;
	
	$offset_width  = $self->{width}  - $offset_col - 1 if $offset_col + $offset_width  >= $self->{width}-1;
	$offset_height = $self->{height} - $offset_row - 1 if $offset_row + $offset_height >= $self->{height}-1;

	my $cls = $DISABLE_ATTRIBUTES?'':CLEAR;

	if($offset_width == 1 && $offset_height == 1)
	{
		return $cls
			.(defined $dc ? "\x1B[${dr};${dc}H" : '')
			._attr($self->{attr}->[$offset_row]->[$offset_col])
			.$self->{data}->[$offset_row]->[$offset_col]
			.$cls
			.(defined $dc?'':"\n\r");
	}
	

	
	my $data = $self->{data};
	my $attr = $self->{attr};
	
	#print '+', '-' x $self->{width}, '+', "\n";
	my $a = time;
	
	my ($row,$line,$col,$chr,$arow,$atr,$lat);
	
	#$lat = $self->{A_all_off};
	#print "Test:\n";
	#print "[". Dumper(\@attr)."]\n";
	
	#@_=();
	
	#push @_, CLEAR;
	

	
	my $sh = $offset_height;
	my $sw = $offset_width;
	my $off = $self->{A_all_off};
	
	my $col2 = $sw + $offset_col;
	my $row2 = $sh + $offset_row;
	
	#print STDERR "to_string: dr=$dr,dc=$dc,row1=$offset_row,row2=$row2,col1=$offset_col,col2=$col2,sh=$sh,sw=$sw\n";
	
	
	my @tmp = ('') x $sh;
	my $tpos = 0;
	$tmp[$tpos++] = $cls;
	
	foreach $row ($offset_row..$row2)
	{
		if(defined $dc)
		{
			$tmp[$tpos++] = "\x1B[".($dr+($row-$offset_row)).";${dc}H" . 
				iloop($data->[$row] || [],$attr->[$row] || [],
				      $offset_col,
				      $col2,
				      $sw,
				      $off,
				      $blank) . $cls;
		}
		else
		{
			$tmp[$tpos++] = iloop($data->[$row] || [],$attr->[$row] || [],$offset_col,$col2,$sw,$off,$blank) . $cls . "\n\r";
		}
		
		#push @_, "\x1B[".($dr+$row).";${dc}H" if defined $dc;
		
		#$line  = $data->[$row] || [];
		#$arow  = $attr->[$row] || [];
		#print '|';
		#$lat = $off;
		
		#push @_, iloop($data->[$row] || [],$attr->[$row] || [],$sw,$off,$blank);
		
		#push @_, $cls . (defined $dc ? '' : "\n\r");
	}
	my $b = time;
	
	#my $c = $self->{height} * $self->{width};
	#my $r = $self->{height};
	
	my $d = $b - $a;
	
	#print '+', '-' x ($self->{width}), '+', "\n";
	
	#print "d=$d, c=$c,r=$r, avg per chr=",($d/$c),", avg per line=",($d/$r),"\n";
	
	return join '', @tmp;
	
	
	#my $tmp = join '', @_;	
	#$tmp =~s/((?:\x1B\(0[qxlkjmtuwvn]\x1B\(B)+)/_compress_lines($1)/seg;
	#$tmp =~s/((?:\x1B\[\d{1,2}m)+)/_compress_attribs($1)/seg;
	#$tmp;
}



sub telnet_stream
{
	my $self = shift;
	my $g = shift;
	
	
	my $dr = shift || 0;
	my $dc = shift || 0;

	my $blank = undef;
	
	my $offset_row    = shift || 0;
	my $offset_col    = shift || 0;
	my $offset_height = shift || $self->{height} - 1;
	my $offset_width  = shift || $self->{width}  - 1;
	
	$offset_row = 0 if $offset_row < 0;
	$offset_col = 0 if $offset_col < 0;
	
	$offset_width  = $self->{width}  - $offset_col - 1 if $offset_col + $offset_width  >= $self->{width}-1;
	$offset_height = $self->{height} - $offset_row - 1 if $offset_row + $offset_height >= $self->{height}-1;

	my $cls = $DISABLE_ATTRIBUTES?'':CLEAR;


	
	my $data = $self->{data};
	my $attr = $self->{attr};
	
	
	my ($row,$line,$col,$chr,$arow,$atr,$lat);
	
	my $sh = $offset_height;
	my $sw = $offset_width;
	my $off = $self->{A_all_off};
	
	my $col2 = $sw + $offset_col;
	my $row2 = $sh + $offset_row;
	
	#print STDERR "to_string: dr=$dr,dc=$dc,row1=$offset_row,row2=$row2,col1=$offset_col,col2=$col2,sh=$sh,sw=$sw\n";
	
	#"\x1B[${row};${col}H"
	foreach $row ($offset_row..$row2)
	{
		iloop2($g,$dr+($row-$offset_row), $dc, $data->[$row] || [],$attr->[$row] || [],
		      $offset_col,
		      $col2,
		      $sw,
		      $off,
		      $blank, $cls); # . $cls;
		
	}
	
	
}


sub iloop2($$$$$$$$$$)
{
	my ($g,$abs_row,$dc,$line,$arow,$offset_col,$col2,$sw,$lat,$blank, $cls) = @_;
	my ($atr,$chr,$col,$cur_attribs);
	my @a = ('') x $sw;
	#print STDERR "$#a=$sw\n";
	
	#
	
	#$lat = $lat->[AS_BG] + $lat->[AS_FG] + $lat->[AS_U] + $lat->[AS_BOLD] + $lat->[AS_BLINK];
	$lat = join '', $lat->[AS_BG] , $lat->[AS_FG] , $lat->[AS_U] , $lat->[AS_BOLD] , $lat->[AS_BLINK];
	
	my $in_data = 0;
	my $has_data = 0;
	
	my $x;
	for $col ($offset_col..$col2)
	{
		$chr = $line->[$col];
		$atr = $arow->[$col];

		#$cur_attribs = $atr->[AS_BG] + $atr->[AS_FG] + $atr->[AS_U] + $atr->[AS_BOLD] + $atr->[AS_BLINK];
		$cur_attribs = join '', $atr->[AS_BG] , $atr->[AS_FG] , $atr->[AS_U] , $atr->[AS_BOLD] , $atr->[AS_BLINK];
		
		
		if(defined $chr)
		{
			
			if(!$in_data)
			{
				$a[$x++] = "\x1B[${abs_row};".($dc+$col)."H";
			}
			
			$a[$x++] = _attr($atr) if $cur_attribs ne $lat;
			$a[$x++] = $chr;
			
			$has_data = 1;
			$in_data  = 1;
		
			
		
		}
		else
		{
			$in_data = 0;
		}

		$lat = $cur_attribs;

	}
	#return join '', @a;
	$g->send(join ('', @a, $cls)) if $has_data;
}




sub _compress_lines($)
{
	local $_= shift;
	
	#print STDERR "----> _compress_lines: in=[$x]\n";
	s/\x1B\([B0]//g;
	#print STDERR "----> _compress_lines: out=[$x]\n";
	"\x1B(0$_\x1B(B"
}

sub _compress_attribs($)
{
	local $_= shift;
	
	#print STDERR "----> _compress_lines: in=[$x]\n";
	@_ = /\x1B\[(\d{1,2})m/g;
	#print STDERR "----> _compress_lines: out=[$x]\n";
	"\x1B[".join(';',@_).'m';
}


sub locate
{
	my $self = shift;
#	print called_from."\n";
#	shift->{cursor} = [shift,shift];
	$self->{cursor} = [shift,shift];
}

sub cursor { @{shift->{cursor}||[]} }

sub print
{
	my $self = shift;
	my $str = shift;
	my $len = ansi_split $str;
	
	@_ = split/\n/, $str;
	
	my $a = $self->{cursor}->[CUR_COL];
	for(0..$#_)
	{
		$self->string($self->{cursor}->[CUR_ROW],$self->{cursor}->[CUR_COL],$_[$_]);
		if($_ < $#_)
		{
			$self->{cursor}->[CUR_ROW]++;
			$self->{cursor}->[CUR_COL]=$a;
		}
	}
	
	$self->{cursor}->[CUR_COL] += $len;
}

sub chrat
{
	shift->{data}->[shift]->[shift];
}

sub attrat
{
	my $self = shift;
	my $row = shift;
	my $col = shift;
	return $self->{attr}->[$row]->[$col] || undef;
}

sub attron
{
	my $self = shift;
	
	my ($d,$r,$k,$v);
	#print STDERR called_from().": num: ".($#_+1)."\n";
	while(@_) #my $attr = shift)
	{
		my $attr = shift;
		next if !defined $attr;
		
		if(ref $attr eq 'ARRAY')
		{
			$self->attron(@$attr);
			next;
		}
		
		
		$d = $attr;
		$d =~s/[^\d]//g;
		
		
			# Modifiers
		$k =    $d == 1 ? AS_BOLD :	# bold
			$d == 2 ? AS_BOLD :	# bold
			$d == 4 ? AS_U :	# u
			$d == 5 ? AS_BLINK :	# blink
			
			# Colors
			$d >= 30 && $d <= 37 ? AS_FG :
			$d >= 40 && $d <= 47 ? AS_BG :
			
			# Unknown
			undef;
			
			# Flags for Modifiers
		$v =    $d == 1 ? 1 :
			$d == 2 ? 0 :
			$d == 4 ? 1 :
			$d == 5 ? 1 :
			
			# Color code
			$k == AS_BG || $k == AS_FG ? $d :
			
			# Unknown
			undef;

		#print STDERR "  attron:$d, k=$k, v=[${v}#".CLEAR."]\n";
		
		$self->{A}->[$k] = $v if defined $k;
	}
	
	return $self->{A};
}

sub attroff
{
	my $self = shift;
	
	if(@_ == 0)
	{
		$self->{A} = undef; #$self->{A_all_off};
		return $self->attr;
	}
	
	my ($d,$r,$k,$v);
	while(@_) #my $attr = shift)
	{
		my $attr = shift;
		next if !defined $attr;

		if(ref $attr eq 'ARRAY')
		{
			$self->attroff(@$attr);
			next;
		}
		
		$d = $attr;
		$d =~s/[^\d]//g;
		
		if($d == 0)
		{
			$self->{A} = undef;
			next;
		}
		
			# Modifiers
		$k =    $d == 1 ? AS_BOLD :
			$d == 2 ? AS_BOLD :
			$d == 4 ? AS_U :
			$d == 5 ? AS_BLINK :
			
			# Colors
			$d >= 30 && $d <= 37 ? AS_FG :
			$d >= 40 && $d <= 47 ? AS_BG :
			
			# Unknown
			undef;
			
			# Flags for Modifiers
		$v =    $d == 1 ? 0 :
			$d == 2 ? 1 :
			$d == 4 ? 0 :
			$d == 5 ? 0 :
			
			# Color
			0;

		
		$self->{A}->[$k] = $v;
	}
	
	return $self->{A};
}

sub  attr { _attr(shift->{A}) }
#,
#s/;+/;/g;s/;m/m/g; 
sub _attr {  
	return if $DISABLE_ATTRIBUTES == 1;
	my $x = shift ;  
	local $_ = join ';', ("\x1B[0",($x->[AS_U]>0?4:0),($x->[AS_BLINK]>0?5:0),$x->[AS_BG],$x->[AS_FG],($x->[AS_BOLD]>0?1:2)); $_.='m'; s/;+/;/g;
	$_  }


sub fill 
{
	my $self = shift;
	my $color = shift;
	
	my $row1 = shift || 0;
	my $col1 = shift || 0;
	
	my $row2 = shift || $self->{height};
	my $col2 = shift || $self->{width};
	
	$row1 = int($row1 + $self->{translate}->{row});
	$col1 = int($col1 + $self->{translate}->{col});

	
	my $clip_r1 = $self->{clip}->{row}  || 0;
	my $clip_r2 = ($self->{clip}->{row2} || $self->{height}) -1;
	my $clip_c1 = $self->{clip}->{col}  || 0;
	my $clip_c2 = ($self->{clip}->{col2} || $self->{width}) -1;
	
	$row1 = $clip_r1 if $row1 < $clip_r1;
	$row2 = $clip_r2 if $row2 > $clip_r2;
	$col1 = $clip_c1 if $col1 < $clip_c1;
	$col2 = $clip_c2 if $col2 > $clip_c2;

	
	my $chr = shift; 
	$chr = ' ' if !defined $chr; #|| ' ';
	
	if($color=~/^\x1B/)
	{
		$self->attron($color);
	}
	elsif(ref $color eq 'ARRAY')
	{
		$self->attron(@$color);
	}
	
	#print STDERR "fill: row1=$row1,col1=$col1 - row2=$row2,col2=$col2\n";
	
	my $at = $self->{A};
	
	my ($rd,$ra);
	for $a ($row1..$row2)
	{
		$self->{data}->[$a]||=[];
		$self->{attr}->[$a]||=[];
		
		$rd = $self->{data}->[$a];
		$ra = $self->{attr}->[$a];
		
		for $b ($col1..$col2)
		{
			$rd->[$b] = ($chr == -1 ? $self->chrat($a,$b)||undef : $chr);
			$ra->[$b] = _atlayer($ra->[$b],$at);
		}
	}
	$self->attroff();
	
	return $self;
}


sub box #($$$$$)
{
	my $self = shift;
	
	my ($row1, $col1, $row2, $col2, $bg, $border, $shadow, $title, $tbg, $txt)  = @_;
	
	my $shaded_border = 0;
	my $bs_bright = '';
	my $bs_shadow = '';
	
	if(ref $bg eq 'HASH')
	{
		my $opts = $bg;
		$bg	= $opts->{bg};#     || WHITE
		$border	= $opts->{border}; # || BLACK;
		$shadow = $opts->{shadow} || 0;
		$title	= $opts->{title};
		$bs_bright = $opts->{border_bright}; # || [BOLD , WHITE];
		$bs_shadow = $opts->{border_shadow}; # || [BLACK];
		
		$shaded_border = 1 if $bs_bright && $bs_shadow  || $opts->{shaded_border};
		
		if($opts->{shaded_border} < 0)
		{
			$shaded_border = 1;
			
			my $t = $bs_shadow;
			$bs_shadow = $bs_bright;
			$bs_bright = $t;
		}
		
		$tbg	= $opts->{title_bg}    || $bg;
		$txt	= $opts->{title_text}  || ($shaded_border ? $bs_bright : $border);
		
		
		
	}
	
	if($row1 < 0)
	{
		$row1 = $self->height/2 - $row2/2;
		$row2 += $row1;
	}
	
	if($col1 < 0)
	{
		$col1 = $self->width/2 - $col2/2;
		$col2 += $col1;
	}
	
	$row1||=0;
	$col1||=0;
	$row2||=$row1+$self->{height}-1;
	$col2||=$col1+$self->{width}-1;
	
	#$border ||= BLACK;
	#$bg     ||= ON_WHITE;
	
	$tbg 	||= $bg;
	$txt 	||= $border;
	#$
	
	#print STDERR "$row1,$col1 - $row2,$col2\n";
	
	my $BoxWidth = $col2 - $col1 + 1;
	
	#$self->fill([ON_BLACK,DIM],$row1+1,$col1+1,$row2+1,$col2+1,-1) if $shadow;
	if($shadow)
	{
		$self->fill([ON_BLACK,DARK],$row1+1,$col2+1,$row2+1,$col2+1,-1);
		$self->fill([ON_BLACK,DARK],$row2+1,$col1+1,$row2+1,$col2+1,-1);
	}
	
	if(defined $border)
	{
		$self->locate($row1,$col1);
		$self->attron($shaded_border ? $bs_bright : $border,$bg);
		$self->print(ULC);

		if($title)
		{
			my $str = $title;
			#$str = "The IT Department's Tools and Tric...";
			my $len = ansi_split($str);
			#print STDERR "len=$len, this->width=".($this->width-4)."\n";
			my $space = 6 + ($shadow?1:0);
			if($len > $BoxWidth - $space)
			{
				$str = substr($str,0,$BoxWidth - ($space+3)).'...';
				$len = length($str);
			}


			$len += 4;
			my $bw = $BoxWidth- (2); # ($shadow?1:0));
			my $x = ($bw / 2 ) - ( $len / 2 ) ;
			#print STDERR "box title (str=$str, len=$len, bw=$bw) x=$x\n";
			$x = int(sprintf("%.0f",$x));
			my $y = $row1;


			my $sa = 0;
			my $sc = 0;
			my $a = ($x - $sa);
			my $b = $x + $len;
			my $c = ($bw - $b - $sc);

			$self->print((HOR() x $a) . RTE);
			$self->attroff($shaded_border ? $bs_bright : $border,$bg);

			$self->attron($tbg,$txt);
			$self->print(" $str ");
			$self->attroff($tbg,$txt);

			$self->attron($shaded_border ? $bs_bright : $border,$bg);
			$self->print(LTE.(HOR() x $c));

		}
		else
		{
			$self->print(HOR() x ( $BoxWidth - 2 ) );
		}


		$self->print(URC);
		#$self->attron($border,$bg);

		$self->attroff($shaded_border ? $bs_bright : $border,$bg);
	}
	
	
	my $offset = defined $border ? 1 : 0;
	for(my $a = $row1 + $offset;$a<=$row2 - $offset; $a++)
	{
		if($offset)
		{
			$self->attron($shaded_border ? $bs_bright : $border,$bg);
			$self->string($a,$col1,VER);
			$self->attroff($shaded_border ? $bs_bright : $border,$bg);
		}
		
		if(defined $bg)
		{
			#print STDERR called_from().":border=".pansi($border).", bg=".pansi($bg)." <<$a,$col1+$offset,(' ' x ($BoxWidth-($offset*2)))>>\n";
			$self->attron($border,$bg);
			$self->string($a,$col1+$offset,(' ' x ($BoxWidth-($offset*2))));
			#$self->fill($bg,$a,$col1+1,1,$BoxWidth-2);
			#$self->attroff($border,$bg);
		}
		
		if($offset)
		{
			$self->attron($shaded_border ? $bs_shadow: $border,$bg);
			$self->string($a,$col1+$BoxWidth-1,VER);
			$self->attroff($shaded_border ? $bs_shadow: $border,$bg);
		}
	}
	
	if($offset)
	{
		$self->locate($row2, $col1);
		$self->attron($shaded_border ? $bs_bright : $border,$bg);
		$self->print(LLC);
		$self->attroff($shaded_border ? $bs_bright : $border,$bg);
		#ON_WHITE,BLACK); #
		$self->attron($shaded_border ? $bs_shadow: $border,$bg);
		$self->print(HOR() x ($BoxWidth - 2));
		$self->print(LRC);

		
	}
	$self->attroff();
	
	return $self;
}

sub print_demo1
{
	
	use Benchmark;
    	my $t0 = new Benchmark;
    
	my $can = __PACKAGE__->new(10,40);
	
	#$can->string(0,2,"<r>H<y>e<g>l<b>l<c>o<m>, <r>W<y>o<g>r<b>l<c>d<m>!");
	#$can->attroff;
	#$can->fill('blue'); #,0,0,$can->height,$can->width,-1);
	
	my $text = <<'EOT';
Fourscore and seven years ago our fathers brought forth on this
continent a new nation, conceived in liberty and dedicated to the
proposition that all men are created equal.

Now we are engaged in a great civil war,   testing whether that nation
or any nation so conceived and so dedicated can long endure. We are
met on a great battlefield of that war. We have come to dedicate a
portion of that field as a final resting-place for those who here gave
their lives that that nation might live. It is altogether fitting and
proper that we should do this.

But in a larger sense, we cannot dedicate, we cannot consecrate, we
cannot hallow this ground.  The brave men, living and dead who
struggled here have consecrated it far above our poor power to add or
detract. The world will little note nor long remember what we say
here, but it can never forget what they did here. It is for us the
living rather to be dedicated here to the unfinished work which they
who fought here have thus far so nobly advanced. It is rather for us
to be here dedicated to the great task remaining before us--that from
these honored dead we take increased devotion to that cause for which
they gave the last full measure of devotion--that we here highly
resolve that these dead shall not have died in vain, that this nation
under God shall have a new birth of freedom, and that government of
the people, by the people, for the people shall not perish from the
earth.
EOT
	
	# Merge it all into one line per paragraph:
	$text =~ s/\n(?=\S)/ /g;
	$text =~ s/\n /\n\n/g;
	
	use Text::Wrapper;
	my $wrapper = Text::Wrapper->new(columns=>$can->width);
	my @lines = split/\n/, $wrapper->wrap($text);
	
	#print $text,"\n\n";
	
	my $t1 = new Benchmark;
	my $td = timediff($t1, $t0);
    	#print "mark1:",timestr($td),"\n";
    
	$can->fill(ON_BLUE); #,0,0,$can->height-5,$can->width-10,-1);
	
	$can->attron(BOLD);
	for(0..$#lines)
	{
		$can->string($_,0,$lines[$_]);
	}
	$can->attroff(BOLD);
	
	$can->fill([ON_GREEN,BOLD],2,2,$can->height-5,$can->width-7,-1);
	
	$can->box(0,4,$can->height-3,$can->width-11,ON_WHITE,BLACK,1,"What is This?",ON_WHITE,[RED]);
	
	$can->attroff;
	
	$can->locate(1,5);
	$can->print("This has been a demo of\nTerm::ANSICanvas\n\nThank you for watching.\nEmail:\njosiahbryan\@gmail.com");
	
	my $t2 = new Benchmark;
	$td = timediff($t2, $t1);
	#print "mark2:",timestr($td),"\n";

	print "\n".REVERSE().'[ Demo1 ]'.CLEAR()." >> [ Original Feature Test of Colors and copy_canvas() ]\n";
	$can->dump;
	
	my $t3 = new Benchmark;
	$td = timediff($t3, $t2);
	#print "mark3:",timestr($td),"\n";


	print "\n\nSection of canvas above from (7,4)-(10,10):\n";
	my $c2 = $can->copy_canvas(7,4,10,10);
	$c2->dump;
	print "\n\n";
	
	my $t4 = new Benchmark;
	$td = timediff($t4, $t3);
	#print "mark4:",timestr($td),"\n";

}

sub render_canvas 
{
	my $self = shift;
	
	my $row = shift || 0;
	my $col = shift || 0;
	
	my $c2   = shift;
	
	my $h = $c2->height-1;
	my $w = $c2->width-1;
	
	#my $at = $self->{A};
	my $clip_r1 = $self->{clip}->{row}  || 0;
	my $clip_r2 = ($self->{clip}->{row2} || $self->{height}) -1;
	my $clip_c1 = $self->{clip}->{col}  || 0;
	my $clip_c2 = ($self->{clip}->{col2} || $self->{width}) -1;
	
		
	my ($rd,$ra,$cd,$ca,$dy,$dx,$chr);
	for $a (0..$h)
	{
		$dy = $a+$row;
		
		
		next if $dy < $clip_r1 || $dy > $clip_r2;
		
		
		$self->{data}->[$dy]||=[];
		$self->{attr}->[$dy]||=[];
		
		$rd = $self->{data}->[$dy];
		$ra = $self->{attr}->[$dy];
		
		$cd = $c2->{data}->[$a];
		$ca = $c2->{attr}->[$a];
		
		for $b (0..$w)
		{
			$dx = $b+$col;
			
			next if $dx < $clip_c1 || $dx > $clip_c2;
		
			
			$chr = $cd->[$b];
			#print "c2($a,$b)>($dy,$dx): chr=[$chr]\n";
			
			$rd->[$dx] = (!defined $chr ? $self->chrat($dy,$dx)||undef : $chr);
			$ra->[$dx] = _atlayer($ra->[$dx],$ca->[$b]);
		}
	}
	$self->attroff();
}

sub copy_canvas
{
	my $self = shift;
	
	my $row1 = shift || 0;
	my $col1 = shift || 0;
	
	my $row2 = shift || $self->{height};
	my $col2 = shift || $self->{width};
	
	
	my $w = $col2 - $col1;
	my $h = $row2 - $row1;
	
	my $dest = __PACKAGE__->new($h,$w);
	
	my ($rd,$ra,$sy,$sx,$sd,$sa);
	for $a (0..$h)
	{
		$sy = $a + $row1;
		
		$dest->{data}->[$a]||=[];
		$dest->{attr}->[$a]||=[];
		
		$rd = $dest->{data}->[$a];
		$ra = $dest->{attr}->[$a];
		
		$sd = $self->{data}->[$sy];
		$sa = $self->{attr}->[$sy];
		
		for $b (0..$w)
		{
			$sx = $b + $col1;
			$rd->[$b] = $sd->[$sx];
			$ra->[$b] = [ @{$sa->[$sx] || [undef,undef,undef,undef,undef] } ];
		}
	}
	
	return $dest;
}


#print_demo2();
sub print_demo2
{
	
	my $can = __PACKAGE__->new(10,40);
	
	#$can->string(0,2,"<r>H<y>e<g>l<b>l<c>o<m>, <r>W<y>o<g>r<b>l<c>d<m>!");
	#$can->attroff;
	#$can->fill('blue'); #,0,0,$can->height,$can->width,-1);
	
	my $a = time;
	
	my $c = 1;
	
	for(0..$c-1)
	{
		
		$can->fill([ON_YELLOW,BOLD],0,0,$can->height,$can->width,HOR);
		
		#$can->fill([ON_GREEN,DARK],2,2,$can->height-5,$can->width-7,-1);
		
		print "\n".REVERSE().'[ Demo2 ]'.CLEAR()." >> [ Canvas Composition Test with render_canvas() ]\n";
		
		print "Background:\n";
		$can->dump;
		
		my $c2 = __PACKAGE__->new(5,10);
		#$c2->print("12345\n54321");
		#$c2->fill([ON_GREEN,BOLD],0,0,$c2->height,$c2->width,-1);
		$c2->box(0,0,$c2->height-2,$c2->width-2,ON_WHITE,BLACK,1);
		
		print "Secondary Canvas:\n";
		$c2->dump();
		
		#print Dumper $c2->{attr}->[0]->[9];
		#$can->clip(3,5,2,45);
		$can->render_canvas(2,2,$c2);
		
		print "Secondary Canvas on Background:\n";
		
		
		print $can->to_string(), "\n\n";
	}
	
	my $b = time;
	my $d = $b-$a;
	my $v = $d/$c;
	#print "d=$d, v=$v\n";
	
	#my $row = 3;
	#my $col = 3;
	#print "Chr at ($row,$col): [".$can->getchr($row,$col)."]\n";
	
	
}

sub print_demo2b
{
	
	my $can = __PACKAGE__->new(10,40);
	
	#$can->string(0,2,"<r>H<y>e<g>l<b>l<c>o<m>, <r>W<y>o<g>r<b>l<c>d<m>!");
	#$can->attroff;
	#$can->fill('blue'); #,0,0,$can->height,$can->width,-1);
	
	my $a = time;
	
	my $c = 1;
	
	my $x = 2;
	my $y = 2;
	for(0..$c-1)
	{
		
		$can->fill([ON_YELLOW,BOLD],0,0,$can->height,$can->width,HOR);
		
		#$can->fill([ON_GREEN,DARK],2,2,$can->height-5,$can->width-7,-1);
		
		print "\n".REVERSE().'[ Demo2 ]'.CLEAR()." >> [ Canvas Composition Test with render_canvas() into a third canvas ]\n";
		
		print "Background:\n";
		$can->dump;
		
		my $c2 = __PACKAGE__->new(5,10);
		#$c2->print("12345\n54321");
		#$c2->fill([ON_GREEN,BOLD],0,0,$c2->height,$c2->width,-1);
		$c2->box(0,0,$c2->height-2,$c2->width-2,ON_WHITE,BLACK,1);
		
		print "Secondary Canvas:\n";
		$c2->dump();
		
		
		my $final = __PACKAGE__->new(10,40);
		$final->render_canvas(0,0,$can);
		$final->render_canvas($x,$y,$c2);
		
		$x++;
		$y++;
		
		#print Dumper $c2->{attr}->[0]->[9];
		#$can->clip(3,5,2,45);
		#$can->render_canvas(2,2,$c2);
		
		print "Final Composite:\n";
		
		
		print $final->to_string(), "\n\n";
		
		#sleep 1;
	}
	
	my $b = time;
	my $d = $b-$a;
	my $v = $d/$c;
	#print "d=$d, v=$v\n";
	
	#my $row = 3;
	#my $col = 3;
	#print "Chr at ($row,$col): [".$can->getchr($row,$col)."]\n";
	
	
}

#print_demo2();
#print_demo1();

sub sign { shift() <0?-1:1 }

sub line
{
	my $self = shift;
	my $attrs = shift;
	my $from_y = shift;
	my $from_x = shift;
	my $y2 = shift;
	my $x2 = shift;
	my $ch = shift || '#';
	
	my ($dx, $dy, $ax, $ay, $sx, $sy, $x, $y,$d);


	$self->attron(ref $attrs ? @$attrs : $attrs);

	$dx = $x2 - $from_x;
	$dy = $y2 - $from_y;

	$ax = abs($dx * 2);
	$ay = abs($dy * 2);

	$sx = sign($dx);
	$sy = sign($dy);

	$x = $from_x;
	$y = $from_y;

	if ($ax > $ay) 
	{
		$d = $ay - ($ax / 2);

		while (1) 
		{
			$self->putchr($y, $x, $ch);
			return if $x >= $x2;

			if ($d >= 0) 
			{
				$y += $sy;
				$d -= $ax;
			}
			$x += $sx;
			$d += $ay;
		}
	} 
	else 
	{
		$d = $ax - ($ay / 2);

		while (1) 
		{
			$self->putchr($y, $x, $ch);
			return if $y >= $y2;

			if ($d >= 0) 
			{
				$x += $sx;
				$d -= $ay;
			}
			$y += $sy;
			$d += $ax;
		 }
	}
	
	$self->attroff;
}


sub print_demo3
{
	
	my $can = __PACKAGE__->new(23,80);

	

	my $x =0;
	my $y =0;
	my $ox = 3;
	my $oy = 3;
		
		
	my $dy = 1;
	my $dx = 2;
	
	my $count =0;
	
	my $a = time;
	while(++$count<350)
	{
		
		$can->clear;
		$can->line([ON_BLUE,RED],$y,$x,$y+($oy * sign($dy)),$x+($ox * sign($dx)),' ');
		$x+=$dx;
		$y+=$dy;
		
		#$x=$y=0 if $x > $can->width-1 || $y > $can->height-1;
		$dx = -2 if($x>$can->width-1);
		$dy = -2 if($y>$can->height-1);
		$dx = 2 if $x<0;
		$dy = 2 if $y < 0;
		
		print CURSOR_OFF . $can->to_string(0,0,'.') . CURSOR_ON;
		
		
		my $b = time;
		my $d = $b-$a;
		my $fps = $count/$d;
		
		print "\n$y,$x  ($dx,$dy) (".$can->height.",".$can->width.") [frame $count] [".sprintf("%.02f fps",$fps)."]      \n";
		
		
		sleep 1/60;
	}
	
	my $b = time;
	my $d = $b-$a;
	my $fps = $count/$d;
	
	print "\n".REVERSE().'[ Demo3 ]'.CLEAR()." >> [ FPS Test ]\n";
	print "d=$d, fps=$fps\n";
	
	

}


#print_demo1();

sub print_demo4()
{
	eval 'use GD;';
	GD::Image->trueColor(1);
	
	my $can = __PACKAGE__->new(24,80);
	
	my $image = GD::Image->new('/appcluster/tgui/pcilogo-verysmall2.png'); #pcilogo-verysmall2.png');
	
	my ($w,$h) = $image->getBounds;
	
	$can->fill(ON_BLUE);
	
	my $m = 255*3/2;
	for my $y (0..$h-1)
	{
		for my $x (0..$w-1)
		{
			my @a= $image->rgb($image->getPixel($x,$y));
			
			if($a[0]+$a[1]+$a[2]>$m)
			{
				$can->attron(BOLD,ON_WHITE,REVERSE);
				$can->string($y,$x,' ');
			}
			else
			{
				$can->attron(ON_BLACK);
				$can->string($y,$x,' ');
			}
		}
	}
	
	$can->string(18,1,"<onblue><white><bold>                     Welcome to Productive Concepts, Inc.                     ");
	$can->string(19,1,"<onblack><red><dim>Authorization required to access the PCI Mainframe.");
	$can->string(21,1,"<green><bold>Login:");
	
	print "\n".REVERSE().'[ Demo4 ]'.CLEAR()." >> [ Image to ANSI Demo ]\n";
	$can->dump;
}

sub resize
{
	my $self = shift;
	
	my $h = shift;
	my $w = shift;
	
	$self->{width} = $w;
	$self->{height} = $h;
	
}


sub print_demo5()
{
	my $can = __PACKAGE__->new(10,40);
	$can->fill([ON_BLUE,DIM]); #,0,0,$can->height,$can->width,HOR);
	
	my $c2 = __PACKAGE__->new(1,40);
	$c2->string(0,0,"2007-07-06 15:59:02  Josiah Bryan");
	$can->render_canvas(0,0,$c2);
	
	
	my $c3 = __PACKAGE__->new(5,32);
	$c3->box(0,0,3,30,{bg=>ON_WHITE,shadow=>1,shaded_border=>1,title=>'ADVICE'});
	
	my $c4 = __PACKAGE__->new(2,29);
	$c4->print("Any advice from the Monks? Rewrite in C using Inline::C? Any perl tricks to
optimize the iloop to make it go faster? Is there something I should do  to
clean up the code? My apologies for asking what seems to me to  be a rather");
	$c3->render_canvas(1,1,$c4);

	
	$can->render_canvas(3,3,$c3);
	
	
	
	
	#$can->box(0,0,$can->height-1,$can->width-1,ON_WHITE,BLACK,1,"PCI Manufacturing System");
	
	
	
	#$can->resize(5,80);
	
	#$can->string(0,20,TTE);
	#$can->string(1,20,VER);
	#$can->string(2,20,BTE);
	
	#$can->string(0,35,TTE);
	#$can->string(1,35,VER);
	#$can->string(2,35,BTE);
	
	print "\n".REVERSE().'[ Demo5 ]'.CLEAR()." >> [ Multiple Canvas Composing and to_string() Subset ]\n";
	$can->dump;
	
	print "to_string() called at 3,3 with size 5 x 10:\n";
	print $can->to_string(undef,undef,undef,3,3,5,10);
		
}


sub print_demo6()
{
	my $can = __PACKAGE__->new(10,80);
	$can->fill([ON_BLUE,DIM]); #,0,0,$can->height,$can->width,HOR);
	
	$can->translate(3,5);
	$can->clip(3,5,2,45);
	$can->fill([ON_GREEN,DIM]); #,0,0,$can->height,$can->width,HOR);
	$can->string(0,0,'At 0,0: This line is translated to 5,5 and clipped to 5,5,3,38');
	$can->string(1,0,'At 1,0: The fill green fill was the whole canvas but it was clipped');	
	$can->string(2,0,'At 2,0: This text will never be drawn');
	$can->clip_off;
	
	$can->attroff();
	#$can->{A} = undef;
	
	$can->string(3,0,'At 3,0: This line is outside the clipping area, but translate is till on ');
	
	$can->clip(3,5,20,5);
	$can->box(4,1,5,10,ON_WHITE,BLACK);
	
	$can->clip_off;
	$can->string(4,5,'<--This box was drawn 4,1,5,10 but clipped by 3,5,20,5');
	
	$can->translate_off;
	
	$can->string(0,0,'At 0,0: Translate and clipping are now off');
	
	#$can->box(2,4,5,50,undef,WHITE);
	
	print "\n".REVERSE().'[ Demo6 ]'.CLEAR()." >> [ Clipping/Translation Demo ]\n";
	$can->dump;
	
		
}

sub hline
{
	my $self = shift;
	my ($col1,$col2,$row,$tends,$ch) = @_;
	$ch ||= HOR;
	
	my $offset = $tends ? 1:0;
	my ($ca,$cb) = ref $tends eq 'ARRAY' ? @$tends : (LTE,RTE);
	
	$self->locate($row,$col1);
	
	my $w = $col2 - $col1 - ($tends * 2);
	
	$self->print($ca) if $tends;
	$self->print($ch x $w);
	$self->print($cb) if $tends;
	
}

sub vline
{
	my $self = shift;
	my ($row1,$row2,$col,$tends,$ch) = @_;
	
	$ch ||= VER;
	
	my $offset = $tends ? 1:0;
	my ($ca,$cb) = ref $tends eq 'ARRAY' ? @$tends : (TTE,BTE);
	
	my $h = $row2 - $row1 - ($tends * 2);
	
	my $row = $row1;
	$self->putchr($row++,$col,$ca) if $tends;
	$self->putchr($row++,$col,$ch) for 0..$h;
	$self->putchr($row++,$col,$cb) if $tends;
	
}



sub print_demo7()
{
	my $can = __PACKAGE__->new(10,80);
	$can->fill([ON_BLUE,DIM]); #,0,0,$can->height,$can->width,HOR);
	
	$can->hline(2,10,2,1);
	
	$can->vline(3,7,2,1);
	
	$can->box(2,10,8,20,undef,WHITE);
	#$can->vline(2,8,19,1);
	
	$can->attron(ON_WHITE);
	$can->vline(4,6,20,undef,' ');
	$can->attroff();
	
	$can->string(2,20,'<ow><black>^<clear>');
	$can->string(8,20,'<ow><black>v<clear>');
	
	
	print "\n".REVERSE().'[ Demo7 ]'.CLEAR()." >> [ Lines ]\n";
	$can->dump;
}


sub print_debug_demo
{
#EAS::Telnet::GUI::Painter=HASH(0x9889a40)->text('<BLACK>','<onYELLOW>',0,0,a)
#pre=[a],chr=[b],post=[c], cx=1,cw=1
#EAS::Telnet::GUI::Painter=HASH(0x9889a40)->text('<YELLOW>','<onBLACK>',1,0,b)
#EAS::Telnet::GUI::Painter=HASH(0x9889a40)->text('<BLACK>','<onYELLOW>',2,0,c)
	my $can = __PACKAGE__->new(1,3);
	$can->fill([ON_BLUE,DIM]); #,0,0,$can->height,$can->width,HOR);
	$can->string(0,0,'<BLACK><onYELLOW>a');
	$can->string(0,1,'<YELLOW><onBLACK>b');
	$can->string(0,2,'<BLACK><onYELLOW>c');
	
	
	print "\n".REVERSE().'[ DEBUG ]'.CLEAR()." >> [ Debug ]\n";
	$can->dump;
	
}

sub scroll_canvas
{
	my $self = shift;
	
	my $dir  = shift || 'up';
	my $amount = shift || 1;
	
	# Up/down are the only ones supported right now 
	
	my $h = $self->{height};
	my $w = $self->{width};
	
	
	if($dir eq 'up')
	{
		for $a (0..$h-1)
		{
			#next if $a <= $amount;
			if($a < $h - $amount)
			{
				$self->{data}->[$a] = $self->{data}->[$a+$amount];
				$self->{attr}->[$a] = $self->{attr}->[$a+$amount];
			}
			else
			{
				$self->{data}->[$a] = [];
				$self->{attr}->[$a] = [];
			}
		}
	}
	elsif($dir eq 'down')
	{
		# -1 because say $h is 10, but at 10 that would be 11 lines...think about it, zero-based arrays, remember?
		for my $a1 (0..$h-1)
		{
			my $a = $h-$a1-1;
			
			if($a >= $amount)
			{
				$self->{data}->[$a] = $self->{data}->[$a-$amount];
				$self->{attr}->[$a] = $self->{attr}->[$a-$amount];
			}
			else
			{
				$self->{data}->[$a] = [];
				$self->{attr}->[$a] = [];
			}
		}
	}
	
	return $self;
}

sub print_demo10
{
	my $can = __PACKAGE__->new(10,40);
	
	#$can->string(0,2,"<r>H<y>e<g>l<b>l<c>o<m>, <r>W<y>o<g>r<b>l<c>d<m>!");
	#$can->attroff;
	#$can->fill('blue'); #,0,0,$can->height,$can->width,-1);
	
	my $text = <<'EOT';
Fourscore and seven years ago our fathers brought forth on this
continent a new nation, conceived in liberty and dedicated to the
proposition that all men are created equal.

Now we are engaged in a great civil war,   testing whether that nation
or any nation so conceived and so dedicated can long endure. We are
met on a great battlefield of that war. We have come to dedicate a
portion of that field as a final resting-place for those who here gave
their lives that that nation might live. It is altogether fitting and
proper that we should do this.

But in a larger sense, we cannot dedicate, we cannot consecrate, we
cannot hallow this ground.  The brave men, living and dead who
struggled here have consecrated it far above our poor power to add or
detract. The world will little note nor long remember what we say
here, but it can never forget what they did here. It is for us the
living rather to be dedicated here to the unfinished work which they
who fought here have thus far so nobly advanced. It is rather for us
to be here dedicated to the great task remaining before us--that from
these honored dead we take increased devotion to that cause for which
they gave the last full measure of devotion--that we here highly
resolve that these dead shall not have died in vain, that this nation
under God shall have a new birth of freedom, and that government of
the people, by the people, for the people shall not perish from the
earth.
EOT
	
	# Merge it all into one line per paragraph:
	$text =~ s/\n(?=\S)/ /g;
	$text =~ s/\n /\n\n/g;
	
	use Text::Wrapper;
	my $wrapper = Text::Wrapper->new(columns=>$can->width);
	my @lines = split/\n/, $wrapper->wrap($text);
	
	#print $text,"\n\n";
	
	#my $t1 = new Benchmark;
	#my $td = timediff($t1, $t0);
    	#print "mark1:",timestr($td),"\n";
    
	$can->fill(ON_BLUE); #,0,0,$can->height-5,$can->width-10,-1);
	
	$can->attron(BOLD, WHITE);
	for(0..$#lines)
	{
		$can->string($_,0,$lines[$_]);
	}
	$can->attroff(BOLD);
	
# 	$can->fill([ON_GREEN,BOLD],2,2,$can->height-5,$can->width-7,-1);
# 	
# 	$can->box(0,4,$can->height-3,$can->width-11,ON_WHITE,BLACK,1,"What is This?",ON_WHITE,[RED]);
# 	
# 	$can->attroff;
# 	
# 	$can->locate(1,5);
# 	$can->print("This has been a demo of\nTerm::ANSICanvas\n\nThank you for watching.\nEmail:\njosiahbryan\@gmail.com");
# 	
# 	my $t2 = new Benchmark;
# 	$td = timediff($t2, $t1);
# 	#print "mark2:",timestr($td),"\n";
# 
# 	print "\n".REVERSE().'[ Demo1 ]'.CLEAR()." >> [ Original Feature Test of Colors and copy_canvas() ]\n";
# 	$can->dump;
# 	
# 	my $t3 = new Benchmark;
# 	$td = timediff($t3, $t2);
# 	#print "mark3:",timestr($td),"\n";
# 
# 
# 	print "\n\nSection of canvas above from (7,4)-(10,10):\n";
# 	my $c2 = $can->copy_canvas(7,4,10,10);
# 	$c2->dump;
# 	print "\n\n";
# 	
# 	my $t4 = new Benchmark;
# 	$td = timediff($t4, $t3);
# 	#print "mark4:",timestr($td),"\n";
	print "\n".REVERSE().'[ Demo10 ]'.CLEAR()." >> [ scroll_canvas() test ]\n";
	$can->dump;
	print "-------\n";
	
	print "\n\nAfter scroll:\n";
	$can->scroll_canvas('up');
	$can->dump;
	print "-------\n";

}

#print_debug_demo();

# sub print_demo8()
# {
# 	my $can = __PACKAGE__->new(12,80);
# 	$can->fill([ON_BLUE,DIM]); #,0,0,$can->height,$can->width,HOR);
# 	
# 	use Term::ANSICanvas::LargeFonts;
# 	#Term::ANSICanvas::LargeFonts::big_string($can,1,1,"Hello, Font # 0",0);
# 	Term::ANSICanvas::LargeFonts::big_string($can,5,5,"Hello, Font # 1",1,ON_WHITE);
# 	
# 	
# 	
# 	
# 	print "\n".REVERSE().'[ Demo8 ]'.CLEAR()." >> [ Large Fonts ]\n";
# 	$can->dump;
# }

# sub print_demo9()
# {
# 	my $can = __PACKAGE__->new(22,80);
# 	$can->fill([ON_BLUE,DIM]); #,0,0,$can->height,$can->width,HOR);
# 	
# 	use Term::ANSICanvas::LargeFonts;
# 	#Term::ANSICanvas::LargeFonts::big_string($can,1,1,"Hello, Font # 0",0);
# 	
# 
# 	
# 	my @lines = split /\n/, q{VORTEC 8100 V8 SFI|                  |                  |VORTEC 8100 V8 SFI|                  |                  |                  |                  |                  |                  |                  |                  |
#   |L18               |                  |                  |L18               |                  |                  |                  |                  |                  |                  |                  |                  |
#   |                  |                  |                  |                  |                  |                  |                  |                  |                  |                  |                  |                  |
#   |DOWNPIPW ASM-EXHAU|                  |MUFFLER ASM-EXH, W|DOWNPIPE ASM-EXHAU|                  |                  |                  |                  |                  |                  |                  |                  |
#   |W0003522          |                  |W0006356          |W0003523          |                  |                  |                  |                  |                  |                  |                  |                  |
#   |                  |                  |                  |                  |                  |                  |                  |                  |                  |                  |                  |                  |
#   |                  |                  |PIPE ASM-EXH TAIL |                  |                  |PIPE ASM-EXH TAIL |                  |                  |                  |                  |                  |                  |
#   |                  |                  |W0000216          |                  |                  |W0000216          |                  |                  |                  |                  |                  |                  |
#   |                  |                  |                  |                  |                  |                  |                  |                  |                  |                  |                  |                  |
#   |CONVERTER-EXH ASM |                  |                  |CONVERTER-EXH ASM |                  |                  |                  |                  |                  |                  |                  |                  |
#   |W0007189          |                  |                  |W0007189          |                  |                  |                  |                  |                  |                  |                  |                  |
#   |                  |                  |                  |                  |                  |                  |                  |                  |                  |                  |                  |                  |
#   |                  |                  |                  |                  |                  |                  |                  |                  |                  |                  |                  |                  |
#   |                  |                  |                  |                  |                  |                  |                  |                  |                  |                  |                  |                  | |
# };
# 	
# 	my $row = 8;
# 	my $col = 10;
# 	
# 	Term::ANSICanvas::LargeFonts::big_string($can,2,1,"1033760",1,ON_WHITE);
# 	
# 	$can->attron(ON_GREEN,BLACK);
# 	$can->string(0,0,'Current Job to Build:                                                           ');
# 	
# 	$can->attron(ON_WHITE,BLACK);
# 	my $mark = ((HOR() x 18));
# 	$can->string($row++,$col,ULC().(($mark.TTE()) x 2).$mark.URC());
# 	
# 	my $table = 'east';
# 	
# 	my $blank_count = 0;
# 	foreach my $line (@lines)
# 	{
# 		my @cols = split /\|/, $line;
# 		shift @cols if $cols[0] eq '  ' && @cols;
# 		
# 		my $r1 = $table eq 'east' ? 0 : 3;
# 		my $r2 = $table eq 'east' ? 2 : 5;
# 		
# 		my $blank = join '', @cols[$r1..$r2];
# 		if($blank !~ /[^\s]/)
# 		{
# 			$blank_count ++;
# 		}
# 		else
# 		{
# 			$blank_count = 0;
# 		}
# 		
# 		next if $blank_count > 1;
# 		
# 		$can->string($row++,$col,VER().join(VER(), @cols[$r1..$r2]).VER());
# 	}
# 	
# 	$can->string($row++,$col,LLC().(($mark.BTE()) x 2).$mark.LRC());
# 		
# 
# 	
# 	
# 	print "\n".REVERSE().'[ Demo9 ]'.CLEAR()." >> [ Screen Transition Prototype ]\n";
# 	
# 	
# 	use Time::HiRes qw/sleep/;
# 	
# 	#$can->dump;
# 	my $state = 1; # anim
# 	my $sleep = .001;
# 	my $inc = -15;
# 	my $pos = -1;
# 	
# 	while($inc > 0 ?  $pos < 80 : $pos > -80)
# 	{
# 	
# 		#print "pos:$pos\n";
# 		my $c1 = abs($pos);
# 		if($inc < 0)
# 		{
# 			print $can->to_string(5,1,undef,0,$c1,22,80);
# 			print $can->to_string(5,80-$c1,undef,0,0,22,$c1);
# 		}
# 		else
# 		{
# 			print $can->to_string(5,$c1,undef,0,0,22,80-$c1);
# 			print $can->to_string(5,1,undef,0,80-$c1,22,80);
# 		}
# 		
# 		sleep $sleep;
# 		$pos += $inc;
# 	}
# 	
# 	print $can->to_string(5,1,undef,0,0,22,80);
# }


#set_line_mode('text');
#$DISABLE_ATTRIBUTES = 1;

#for(0..10)
#{
	#print_demo1();
#}
# print_demo2b();
# print_demo3();
# print_demo4();
# print_demo5();
# print_demo6();
# print_demo7();
# print_demo8();
# print_demo9();
# print_demo10();
#exit;
1;
