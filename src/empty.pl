#!/usr/bin/env perl

# this script generates a zero sudoku
# of a given dimension. This is useful
# for seeing how good is the algorithm,
# in particular, how good is heuristic.
#
# usage: ./empty.pl 5 | ./program

my $n = (scalar @ARGV) ? (shift @ARGV) : 3;

print "$n\n";
my $s = $n ** 2 - 1;
print (join "\n", map { $_ = (join " ", map { $_ = "0" } (0..$s)) } (0..$s));
