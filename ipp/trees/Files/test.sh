#!/bin/bash


expected () {
	printf '%s %s\n' 'Expected <prog> <dir> or -v <prog> <dir>'
}

# Parse parameters
if [ $# = 2 ]; then
	prog=$1
	dir=$2
elif [ $# = 3 ]; then
	prog=$2
	dir=$3
	if [ "$1" != "-v" ]; then
		echo "Wrong flag"
		expected
		exit 1
	fi
else
	expected 
	exit 1
fi;

if [ ! -e $dir ] || [ ! -d $dir ]; then
	echo "Wrong directory"
	expected
	exit 1
elif [ ! -e $prog ] || [ ! -f $prog ]; then
	echo "Wrong program name"
	expected
	exit 1
fi


myout="TEMP.out"
myerr="TEMP.err"

# Find tests
files=$dir/*.in
files=$(
	for f in $files; do
		echo $f
	done | sort -V)

# Run tests
for i in $files; do
	name=${i%%.in}
	input=$name".in"
	error=$name".err"
	output=$name".out"
	
	if [[ $# = 3 ]]; then
		cprogv="./$prog -v <$input >$myout 2>$myerr"
		err=$(eval $cprogv)
		derr=$(diff $error $myerr)
	else
		cprog="./$prog <$input >$myout"
		err=$(eval $cprog)
		derr=""
	fi
	
	dstd=$(diff $output $myout)
	if [[ $dstd != "" ]] || [[ $dstd != "" ]]; then
		echo $i
	fi
done
rm -f $myout $myerr
