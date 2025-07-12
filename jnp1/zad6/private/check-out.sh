make
./Christmas > my.out
diff my.out ChristmasTree.out

if [ $? -eq 0 ];
then
	echo "OK"
else
	echo "NIE OK"
fi
rm my.out
make clean
