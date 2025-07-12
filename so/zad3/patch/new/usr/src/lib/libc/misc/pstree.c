#include <lib.h>
#include <unistd.h>
#include <sys/types.h>

int pstree(pid_t pid, int uid) {
	message m;
	m.m1_i1 = pid;
	m.m1_i2 = uid;
	m.m1_i3 = getgid();
	return _syscall(PM_PROC_NR, PM_PSTREE, &m);
}
