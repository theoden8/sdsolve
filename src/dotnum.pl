#!/usr/bin/env perl

# this script helps to translate
# the dot-number notation used
# in downloaded sudoku data sets
# into the notation specific to
# this practical.

use feature qw(say);

print <STDOUT>, "3\n";
for(my $i = 0; $i < 9; ++$i) {
  for(my $j = 0; $j < 9; ++$j) {
    my $c = getc(STDIN);
    print <STDOUT>, (($c == '.') ? '0' : $c);
    print <STDOUT>, " ";
  }
  print "\n";
}
