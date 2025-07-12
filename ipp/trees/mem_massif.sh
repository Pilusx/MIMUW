#!/bin/bash

prog=$1
dir=$2

valgrind --tool=massif --massif-out-file=mas.out ./$1 <$dir > te.out

d=$(ms_print mas.out | grep '[A-Z]B')
val=$(ms_print mas.out | grep '[\^]')

echo $val $d

rm -f mas.out
rm -f te.out
