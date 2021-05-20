#ifndef _PROC_SYSCALLS_H_
#define _PROC_SYSCALLS_H_

#include "../compile/LAB2/opt-proc_syscalls.h"
#include <types.h>

void sys__exit(int status);

//LAB4.4
pid_t sys_getpid (void);
pid_t sys_waitpid(pid_t, int *, int);

#endif /* _PROC_SYSCALLS_H_ */