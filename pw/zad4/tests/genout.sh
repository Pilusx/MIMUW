#!/bin/bash

for f in $(ls *.in); do
	pref=${f%%.in}
	echo GENERATED $pref.out
	
	tail -n +2 $f | sort -n > $pref.out

done;
