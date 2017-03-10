#!/usr/bin/env perl
use feature('say'); $_=<>,$n=int sqrt sqrt -1+length
;say$n;$n*=2*$n;s/.\K(?=.)/ /sg;s/.{$n}\K(?=.)/\n/sg
;for$c('A'..'Z'){$d = (ord$c)-(ord'A')+10,s/$c/$d/g;
}s/\./0/g;print
