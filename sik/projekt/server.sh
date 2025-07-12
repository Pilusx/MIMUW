#!/bin/bash

ARG=${HOSTNAME}

DIR=drive/server/haha_$ARG
rm -rf ${DIR}
mkdir -p ${DIR}
echo "serv" > ${DIR}/serv${ARG}e1.txt 
echo "serv2" > ${DIR}/serv${ARG}1e2.txt

./netstore-server -g 225.1.0.0 -p 10003 -f ${DIR} -b 1100000 -t 2 > drive/logs/stdout/server_${ARG} 2> drive/logs/stderr/server_${ARG}
