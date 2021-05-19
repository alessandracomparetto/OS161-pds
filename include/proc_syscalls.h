#ifndef _PROC_SYSCALLS_H_
#define _PROC_SYSCALLS_H_

#include "../compile/LAB2/opt-proc_syscalls.h"
#include <types.h>

void sys__exit(int status);
pid_t sys_getpid (struct proc *);
int sys_waitpid(pid_t);

#endif /* _PROC_SYSCALLS_H_ */