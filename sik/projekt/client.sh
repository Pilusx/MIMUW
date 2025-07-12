#!/bin/bash

ARG=${HOSTNAME}

DIR=drive/client/haha_$ARG

rm -rf ${DIR}
mkdir -p ${DIR}

for i in {1..5}; do echo "hehe ${ARG}/${i}" >> ${DIR}/hehe$ARG.txt; done

echo "hihi" > ${DIR}/hihi$ARG.txt
echo "race"$ARG > ${DIR}/race.txt

mkdir -p drive/test
TESTFILE=drive/test/client${ARG}

echo "?info" >> $TESTFILE
echo "discover" >> $TESTFILE
echo "upload" hehe$ARG.txt >> $TESTFILE
echo "fetch" hihi$ARG.txt >> $TESTFILE
echo "upload" hihi$ARG.txt >> $TESTFILE
echo "fetch" hihi$ARG.txt >> $TESTFILE

# gcc test
echo "upload /usr/bin/gcc" >> $TESTFILE
echo "fetch gcc" >> $TESTFILE

# Race test
echo "upload race.txt" >> $TESTFILE
echo "fetch race.txt" >> $TESTFILE
echo "remove race.txt" >> $TESTFILE
echo "fetch race.txt" >> $TESTFILE
cat test.txt >> $TESTFILE

./netstore-client -g 225.1.0.0 -p 10003 -o ${DIR} -t 1 < $TESTFILE > drive/logs/stdout/client_${ARG} 2> drive/logs/stderr/client_${ARG}
