#include <unistd.h>
#include <lib.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char** argv){

message m;
m.m1_i1 = 1;
m.m1_i2 = 0;
m.m1_i3 = 0; 


_syscall(PM_PROC_NR, PM_PSTREE, &m);
return 2;

if(argc != 3) {
	return 1;
}


int pid = atoi(argv[1]);
int uid = atoi(argv[2]);

pstree(pid, uid);
	return 0;
}
