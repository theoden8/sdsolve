#!/usr/bin/env perl

use feature('say');$_=(0..scalar@ARGV?shift@ARGV:3);say;$_**=2;print(('0 'x$_."\n")x$_);
