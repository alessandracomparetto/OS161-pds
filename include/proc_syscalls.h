#ifndef _PROC_SYSCALLS_H_
#define _PROC_SYSCALLS_H_

#include "../compile/LAB2/opt-proc_syscalls.h"
#include <types.h>

#if OPT_PROC_SYSCALLS
void sys__exit(int status);
#endif

#endif /* _PROC_SYSCALLS_H_ */