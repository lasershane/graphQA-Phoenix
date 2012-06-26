#!c:/perl/bin/perl -w

while(<STDIN>) {
	if ( !/^\[/ ) {next;}
	chop;
	s/\[//g;
	s/\]//g;
	print "$_\n";
}
