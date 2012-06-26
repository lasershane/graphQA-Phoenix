
open OUT, ">", @ARGV[0] or die $!;
shift(@ARGV);

IN: foreach (@ARGV) {

      open(IN, $_) || ((warn "Can't open file $_\n"), next IN);

      while (<IN>) {
         print OUT <IN>;
      }
      close(IN);
   }

close OUT;