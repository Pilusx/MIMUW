#!/bin/bash

names="road_PA"
stats="stats.txt"
rm ${stats}

for count in 10 20 30 40 50 60 70 80 90 100 250 500 750 1000 1250 3000 7000 15000 30000 50000 70000 100000 200000 300000 400000 500000 1000000 15000000; do
	for f in ${names}; do
		name="${f}_${count}.txt"
		head -${count} ${f}.txt > ${name}
		python3 stats.py ${name} >> ${stats}
	done
done
