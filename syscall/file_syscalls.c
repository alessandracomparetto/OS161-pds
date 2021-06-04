/*
*	LAB 02.1
*	due funzioni sys_write() e sys_read() (con parametri che ricalchino i prototipi di read e write ). 
*	Si suggerisce di limitare la realizzazione al caso di IO su stdout e stdin (eventualmente stderr ), 
*	evitando la gestione esplicita dei file (proposta in un successivo laboratorio) e ricorrendo invece 
*	(in quanto si tratta di file testo) alle funzioni putch() e getch() (si veda ad esempio il loro utilizzo in kprintf() e kgets() ).
*/
#include <types.h>
#include <syscall.h>
#include <kern/errno.h>
#include <clock.h>
#include <kern/unistd.h>
#include <lib.h>
#include <limits.h>
#include <current.h>
#include <lib.h>

#include "../include/file_syscalls.h" 


/*******************************
 * LAB 5
 *  struct openfile, systemFiletable, open, close
 *  read, write
*********************************/
#include <vnode.h>
#include <vfs.h>
#include <proc.h>
#include <uio.h>
#include <copyinout.h>
#define SYSTEM_OPEN_MAX (10*OPEN_MAX)
#define USE_KERNEL_BUFFER 0

/* system open file table */
struct openfile
{
	struct vnode *vn;
	off_t offset;
	//unsigned int countRef;
	int fd; //file descriptor
};

struct openfile systemFileTable[SYSTEM_OPEN_MAX]; 

static int
file_read(int fd, userptr_t buf_ptr, size_t size) {
  struct iovec iov;
  struct uio ku;
  int result, nread;
  struct vnode *vn;
  struct openfile *of;
  void *kbuf;

  if (fd<0||fd>OPEN_MAX) return -1;
  of = curproc->fileTable[fd];
  if (of==NULL) return -1;
  vn = of->vn;
  if (vn==NULL) return -1;

  kbuf = kmalloc(size);
  uio_kinit(&iov, &ku, kbuf, size, of->offset, UIO_READ);
  result = VOP_READ(vn, &ku);
  if (result) {
    return result;
  }
  of->offset = ku.uio_offset;
  nread = size - ku.uio_resid;
  copyout(kbuf,buf_ptr,nread);
  kfree(kbuf);
  return (nread);
}

static int
file_write(int fd, userptr_t buf_ptr, size_t size) {
  struct iovec iov;
  struct uio ku;
  int result, nwrite;
  struct vnode *vn;
  struct openfile *of;
  void *kbuf;

  if (fd<0||fd>OPEN_MAX) return -1;
  of = curproc->fileTable[fd];
  if (of==NULL) return -1;
  vn = of->vn;
  if (vn==NULL) return -1;

  kbuf = kmalloc(size);
  copyin(buf_ptr,kbuf,size);
  uio_kinit(&iov, &ku, kbuf, size, of->offset, UIO_WRITE);
  result = VOP_WRITE(vn, &ku);
  if (result) {
    return result;
  }
  kfree(kbuf);
  of->offset = ku.uio_offset;
  nwrite = size - ku.uio_resid;
  return (nwrite);
}

/*
* File system calls for open / close
*/

int
sys_open(userptr_t path, int openflags, mode_t mode, int *errp)
{
  int fd, i;
  struct vnode *v;
  struct openfile *of=NULL;; 	
  int result;

  result = vfs_open((char *)path, openflags, mode, &v);
  if (result) {
    *errp = ENOENT;
    return -1;
  }
  /* search system open file table */
  for (i=0; i<SYSTEM_OPEN_MAX; i++) {
    if (systemFileTable[i].vn==NULL) {
      of = &systemFileTable[i];
      of->vn = v;
      of->offset = 0; // TODO: handle offset with append
    //   of->countRef = 1;
      break;
    }
  }
  if (of==NULL) { 
    // no free slot in system open file table
    *errp = ENFILE;
  }
  else {
    for (fd=STDERR_FILENO+1; fd<OPEN_MAX; fd++) {
      if (curproc->fileTable[fd] == NULL) {
	curproc->fileTable[fd] = of;
	return fd;
      }
    }
    // no free slot in process open file table
    *errp = EMFILE;
  }
  
  vfs_close(v);
  return -1;
}

/*
 * file system calls for open/close
 */
int
sys_close(int fd)
{
  struct openfile *of=NULL; 
  struct vnode *vn;

  if (fd<0||fd>OPEN_MAX) return -1;
  of = curproc->fileTable[fd];
  if (of==NULL) return -1;
  curproc->fileTable[fd] = NULL;

//   if (--of->countRef > 0) return 0; // just decrement ref cnt
  vn = of->vn;
  of->vn = NULL;
  if (vn==NULL) return -1;

  vfs_close(vn);	
  return 0;
}

//il ritorno di entrambe le funzioni è il numero di byte letti

int
sys_write(int fd, userptr_t buf_ptr, size_t size)
{
  int i;
  char *p = (char *)buf_ptr;

  if (fd!=STDOUT_FILENO && fd!=STDERR_FILENO) {
#if OPT_FILE
    return file_write(fd, buf_ptr, size);
#else
    kprintf("sys_write supported only to stdout\n");
    return -1;
#endif
  }

  for (i=0; i<(int)size; i++) {
    putch(p[i]);
  }

  return (int)size;
}

int
sys_read(int fd, userptr_t buf_ptr, size_t size)
{
  int i;
  char *p = (char *)buf_ptr;

  if (fd!=STDIN_FILENO) {
#if OPT_FILE
    return file_read(fd, buf_ptr, size);
#else
    kprintf("sys_read supported only to stdin\n");
    return -1;
#endif
  }

  for (i=0; i<(int)size; i++) {
    p[i] = getch();
    if (p[i] < 0) 
      return i;
  }

  return (int)size;
}

