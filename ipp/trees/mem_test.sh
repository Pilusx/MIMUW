#1/bin/bash

select word in "list" "tree" "solution"
do
	if [ $word = "solution" ]; then
		echo "Which test?"
		read num
		name="main"
		echo "Test->" $name	"_test$num.in"
		if [ ! -e solution ]; then
			exit 1
		fi
		valgrind --error-exitcode=15 --leak-check=full --track-origins=yes ./solution <tests/test$num.in >te.out
		diff tests/test$num.out te.out
		rm -f te.out
	else
		name="test_"$word
		echo "Test-> "$name
		make $name
		$valgr ./$name
	fi
	break
done
exit 0
