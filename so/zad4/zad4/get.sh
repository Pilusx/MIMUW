#!/bin/sh

option=$1

copy_dir()
{
	directory=$1
	
	mkdir -p .${directory}
	echo "("$(($# - 1))")COPYING" ${directory} 
	
	if [ $# -eq 1 ]; then
		return
	fi
	
	for i in $(seq 2 $#);
	do
		file=${directory}/\$$i
		file=$(eval echo $file)
        
        if [ "$option" = "ECHO" ]; then
        echo $file
        elif [ "$option" = "COPY" ]; then
			if [ -f $file ]; then
				cp $file .$file
			else
				touch .$file
				echo "TOUCHED" .$file
			fi
        fi
        #copy[$index]=${!current}
    done
    #echo ${copy[*]}
}

PREFIX=get
mkdir -p ${PREFIX}
cd ${PREFIX}

short_copy_dir () {
	copy_dir ${MAIN_PATH}$@
}


MAIN_PATH=/usr/src

short_copy_dir /minix/kernel "proc.h proc.c system.h system.c"

short_copy_dir /minix/kernel/system "do_schedctl.c do_schedule.c do_schedule_sjf.c"


short_copy_dir /minix/servers/sched "schedproc.h schedule.c"

short_copy_dir /minix/lib/libsys "sys_schedule.c sys_schedctl.c sys_schedule_sjf.c"

#/usr/src/minix/include/minix/syslib.h, który będzie kopiowany do /usr/include/minix/syslib.h
#/usr/src/minix/include/minix/callnr.h, który będzie kopiowany do /usr/include/minix/callnr.h
#/usr/src/minix/include/minix/com.h który będzie kopiowany do /usr/include/minix/com.h,
short_copy_dir /minix/include/minix "syslib.h callnr.h com.h"


# /usr/src/include/unistd.h, który będzie kopiowany do /usr/include/unistd.h
short_copy_dir /include "unistd.h"

short_copy_dir /lib/libc/misc "setsjf.c Makefile.inc"

short_copy_dir /minix/servers/pm "proto.h table.c Makefile schedule.c setsjf.c utility.c"
