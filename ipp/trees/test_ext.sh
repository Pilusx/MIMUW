#!/bin/bash

if [ $# = 2 ]; then
	prog=$1
	dir=$2
elif [ $# = 3 ]; then
	prog=$2
	dir=$3
	if [ "$1" != "-v" ]; then
		exit 1
	fi
else
	exit 1
fi;

myout="TEMP.out"
myerr="TEMP.err"
massout="MAS.out"
massgraph="MAS2.out"
massif="valgrind --tool=massif --massif-out-file=$massout"

printf '%-20s|%-6s|%-6s|%-7s|%-10s\n' "TestName" "STDOUT" "STDERR" "TIME[s]" "HEAP"

files=$dir/*.in
files=$(
	for f in $files; do
		echo ${f##$dir/}
	done | sort -V)

for i in $files; do
	name=$dir/${i%$'.in'}
	printf '%-20s|' $i
	input=$name".in"
	error=$name".err"
	output=$name".out"
		
	start=$(date +%s)

	if [[ $# = 3 ]]; then
		cprogv="./$prog -v <$input >$myout 2>$myerr"
		ERR=$(eval $cprogv)
		DERR=$(diff $error $myerr | grep -w '^>*' | wc -l)
		if [ $DERR -eq 0 ]; then
			TERR="OK"
		else
			TERR="ERROR"
		fi		
	else
		cprog="./$prog <$input >$myout"
		ERR=$(eval $massif $cprog 2>$massgraph)
		rm -f $massgraph
		TERR="-"
	fi
	
	DSTD=$(diff $output $myout | grep -w '^>*' | wc -l)
	
	if [ $DSTD -eq 0 ]; then 
		TOUT="OK"
	else
		TOUT="ERROR"
	fi
		
	#Evaluate time
	end=$(date +%s)
	TIME=$(($end - $start))
	
	#Evaluate heap usage
	if [[ $# == 2 ]]; then
		d=$(ms_print $massout | grep '[A-Z]B')
		d=$(echo $d | head -c 1)
		if [[ d == "" ]]; then
			d="B";
		else
			d=$d"iB";
		fi		
		val=$(ms_print $massout | grep '\^')
		val=${val%%^*}
		HEAP=$(printf '%s[%s]' $val $d)
		rm -f $massout
	else
		HEAP="-"
	fi

	#Print line
	printf '%-6s|%-6s|%-7s|%-10s\n' $TOUT $TERR $TIME $HEAP
	rm -f $myout $myerr
done

