id="bw386389"

function cprint {
	#https://stackoverflow.com/questions/5947742/how-to-change-the-output-color-of-echo-in-linux
	col=$1
	str=$2
	
	color=`tput setaf ${col}`
	reset=`tput sgr0`
	echo "${color}${str}${reset}"
}

function colorprint {
	#https://stackoverflow.com/questions/5947742/how-to-change-the-output-color-of-echo-in-linux
	cprint 6 "=== $1 ..."
}

function dprint {
	cprint 1 "== Currently in $(pwd)"
}

function pack {
	colorprint "Packing"
	cd ${id}
	dprint
	./packme.sh
	
	cd ..
	#dprint
}

function verify {
	colorprint "Verifing"
	cd verify
	dprint
	
	#python verify.py # on students, use 
	python3 verify.py
	
	cd ..
	#dprint
}


function unz {
	colorprint "Unzipping"
	cd sols
	dprint
	
	unzip ${id}.zip;
	mkdir build; 
	cd build; 
	cmake -DCMAKE_BUILD_TYPE=Release ..; 
	make
	
	cd ../..
	#dprint
}

function run {
	colorprint "Running"
	cd sols/build
	dprint
	
	limitB=1
	
	for f in $(ls ../../data/*.txt | sort -V); do
		plikGrafu=$f
		for liczbaWatkow in $(seq 1); do
			echo ./adorate ${liczbaWatkow} ${plikGrafu} ${limitB}
			time ./adorate ${liczbaWatkow} ${plikGrafu} ${limitB}
		done
	done
	
	cd ../../
	#dprint
}



function clean {
	colorprint "Cleaning"
	cd sols
	dprint
	
	rm -r *
	
	cd ..
	#dprint
}

function clean_tests {
	colorprint "Cleaning test directories"
	dprint
	
	for f in tests-*;
	do 
		echo "Removing directory: $f"
		rm -r $f
	done
}

function clean_logs {
	colorprint "Cleaning logs"
	dprint
	rm -r logs
}


pack

verify
clean_tests
#clean_logs

unz
run
clean
colorprint "Done"
