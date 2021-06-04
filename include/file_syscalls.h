#ifndef _FILE_SYSCALLS_H_
#define _FILE_SYSCALLS_H_

#include "../compile/LAB2/opt-file_syscalls.h"
#include <types.h>
#include "../compile/LAB5/opt-file.h"

#if OPT_FILE_SYSCALLS
int sys_write(int fd, userptr_t buf_ptr, size_t size);
int sys_read(int fd, userptr_t buf_ptr, size_t size);
#endif

//#if OPT_FILE
int sys_open(userptr_t path, int openflags, mode_t mode, int *errp);
int sys_close(int fd);
//#endif

#endif /* _FILE_SYSCALLS_H_ */