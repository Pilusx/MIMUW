#!/bin/bash

function generate {
	exp=$1
	n=$((2**$1))
	num=$((2*$n))
	echo $exp "->" $num
	
	random=th${exp}.in
	inverse=inv${exp}.in
	sorted=sort${exp}.in
	
	echo $n > $sorted
	seq 1 $num >> $sorted
	
	echo $n > $random
	seq 1 $num | shuf >> $random
	
	echo $n > $inverse
	seq $num -1 1 >> $inverse
	
		
}

for f in $(seq 0 2 18) 1; do
	generate $f
done;
