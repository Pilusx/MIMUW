#include <lib.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <minix/rs.h>



int get_ipc_endpt(endpoint_t *pt){
	return minix_rs_lookup("ipc", pt);
}


int main(int argc, char** argv){
endpoint_t ipc_pt;
message m;
m.m1_i1 = 192;
m.m1_i2 = 0;
m.m1_i3 = 0; 

if(get_ipc_endpt(&ipc_pt) != 0) {
	errno = ENOSYS;
	return -1;
}


_syscall(ipc_pt, PM_PSTREE, &m);
return 2;

if(argc != 3) {
	return 1;
}


int pid = atoi(argv[1]);
int uid = atoi(argv[2]);

pstree(pid, uid);
	return 0;
}
