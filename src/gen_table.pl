#!/usr/bin/env perl

# this script is supposed to generate
# assign_table.c file with a function
# assign_ctable that would speed up
# three times nested for loop setting
# values in the table with column va-
# lues of size n**6.

my $ind = 0;

sub indent {
  print '  'x$ind;
  print (join ",", @_);
  print "\n";
}

sub dump_ctable {
  my $n = shift @_;
  my $ne2 = $n*$n;
  my $ne4 = $ne2 * $ne2;

  my @c = map [map 0, 1..($ne4*4)], 1..$ne2;
  my $i = 0;
  for(my $r = 0; $r < $n*$n; ++$r) {
    for(my $c = 0; $c < $n*$n; ++$c) {
      for(my $v = 0; $v < $n*$n; ++$v) {
        $c[$i][0] = $ne2 * $r + $c + $ne4*0;
        $c[$i][1] = ($r // $n * $n + $c // $n) * $ne2 + $v + $ne4*1;
        $c[$i][2] = $ne2 * $r + $v + $ne4*2;
        $c[$i][3] = $ne2 * $c + $v + $ne4*3;
        # print indent(map { $c[$i][$_] } (0..3));
        indent("s->c[$i+0] = $c[$i][0];");
        indent("s->c[$i+1] = $c[$i][1];");
        indent("s->c[$i+2] = $c[$i][2];");
        indent("s->c[$i+3] = $c[$i][3];");
        ++$i;
      }
    }
  }
  return @c;
}

sub wrap_ctable {
  my $l = 1, $r = 9;
  # indent("#include <omp.h>");
  # indent();
  indent("void assign_ctable(sd_t *s) {"); ++$ind;
  indent("switch(s->n) {"); ++$ind;
  for(my $n = $l; $n <= $r; ++$n) {
    indent("case $n:"); ++$ind;
    dump_ctable($n); --$ind;
    indent("break;");
  }
  --$ind;indent("}"); --$ind;
  indent("}");
}

wrap_ctable;
