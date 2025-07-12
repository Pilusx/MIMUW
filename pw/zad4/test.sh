#!/bin/bash

prog="sort"
folder="tests"

function test {
	file=$1
	pref=${file%%.in}
	in=$pref.in
	out=$pref.out
	temp=${folder}/"temp"

	echo TESTING $pref
	if [[ -e $out ]]; then
    time ./${prog} < $in > ${temp} 2> /dev/null
		diff $temp $out > /dev/null 2>&1
    error=$?
		if [ $error -ne 0 ]; then
			echo "ERROR"
			exit
		fi
	fi
}

# Powinno działać dla 18
for t in $(seq 0 2 12); do
  name=${folder}/sort${t}.in
	test $name
done

# Powinno działać dla 8
for t in $(seq 0 2 10); do
	name=${folder}/inv${t}.in
	test $name
done

# Powinno działać dla 8
for t in $(seq 0 2 10); do
	name=${folder}/th${t}.in
	test $name
done
