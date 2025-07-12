#!/bin/sh

TYPE=$1
SUFFIX=$2
copys() {
	D=$1	
	FILE=$2
	PREF=${TYPE}
	TO=${PREF}/${D}
	mkdir -p ${TO}
	cp /${D}/${FILE}${SUFFIX} ${TO}/${FILE}
	echo ${TO}/${FILE}
}


path=usr/src/minix
servers=${path}/servers/pm

copys usr/src/lib/libc/misc pstree.c
copys usr/src/lib/libc/misc Makefile.inc
copys usr/include unistd.h
copys usr/src/include unistd.h

copys ${path}/include/minix callnr.h
copys usr/include/minix callnr.h
copys ${servers} proto.h
copys ${servers} table.c
copys ${servers} pstree.c
copys ${servers} Makefile

