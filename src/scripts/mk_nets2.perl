#!c:/perl/bin/perl -w

open(GRA, "ta.gra") or die("Couldn't open ta.gra.");
open(NET, ">nets") or die("Couldn't open nets");

while(<GRA>) {
	if ( !/^\[/ ) {next;}
	chop;
	s/\[//g;
	s/\]//g;
	print NET "$_\n";
}
