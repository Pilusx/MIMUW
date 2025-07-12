#!/bin/bash

dir=$1

if [[ ! -d $dir ]]; then
	exit
fi

FILES=$dir/*.in
FILES=$(
for f in $FILES; do
	echo $f
done | sort -V)


printf '%-20s|%-7s|%-7s|%-7s|%-7s|%-7s|%-7s\n' "Name" "ADD" "DELNOD" "DELSUB" "SPLIT" "RIGHT" "SUM"
for f in $FILES; do
	name=${f##$dir/}
	ADD=$(grep "ADD_NODE" $f | wc -l)
	DELNOD=$(grep "DELETE_NODE" $f | wc -l)
	DELSUB=$(grep "DELETE_SUBTREE" $f | wc -l)
	SPLIT=$(grep "SPLIT" $f | wc -l)
	RIGHT=$(grep "RIGHT" $f | wc -l)
	SUM=$(cat $f | wc -l)
	printf '%-20s|%-7s|%-7s|%-7s|%-7s|%-7s|%-7s\n' $name $ADD $DELNOD $DELSUB $SPLIT $RIGHT	$SUM
done
