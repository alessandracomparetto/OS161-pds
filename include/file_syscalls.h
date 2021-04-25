#ifndef _FILE_SYSCALLS_H_
#define _FILE_SYSCALLS_H_

#include "../compile/LAB2/opt-file_syscalls.h"
#include <types.h>

#if OPT_FILE_SYSCALLS
ssize_t sys_write (int filehandle, const void *buf, size_t size);
ssize_t sys_read (int filehandle, void *buf, size_t size);
#endif

#endif /* _FILE_SYSCALLS_H_ */