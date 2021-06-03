/*
*	LAB 02.1
*	due funzioni sys_write() e sys_read() (con parametri che ricalchino i prototipi di read e write ). 
*	Si suggerisce di limitare la realizzazione al caso di IO su stdout e stdin (eventualmente stderr ), 
*	evitando la gestione esplicita dei file (proposta in un successivo laboratorio) e ricorrendo invece 
*	(in quanto si tratta di file testo) alle funzioni putch() e getch() (si veda ad esempio il loro utilizzo in kprintf() e kgets() ).
*/
#include <types.h>
#include <syscall.h>
#include <kern/unistd.h>
#include <lib.h>
#include <limits.h>

#include "../include/file_syscalls.h" 

/*******************************
 * LAB 5
 *  struct openfile, systemFiletable, open, close
 *  read, write
*********************************/
#include <kern/errno.h> 
#include <vnode.h>
#include <vfs.h>
#include <current.h>
#include <proc.h>
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

/*
* File system calls for open / close
*/

int
sys_open(userptr_t path, int openflags, mode_t mode, int *errp){
	struct vnode *v;
	struct openfile *of = NULL;
	int result;

	result = vfs_open((char *) path, openflags, mode, &v);
	if (result){
		*errp = ENOENT;
		return -1;
	}

	/* search open file system table*/
	for (int i = 0; i<SYSTEM_OPEN_MAX; i++){
		if(systemFileTable[i].vn == NULL){
			of = &systemFileTable[i];
			of->vn = v;
			of->fd = i;
			of->offset = 0; //lmao
			break;
		}
	}
	if(of==NULL){
		//no free slot
		*errp = ENFILE;
	}else{
		/*add in perprocess file table*/
		for(int i = 0; i < OPEN_MAX; i++){
			if (curproc->fileTable[i] == NULL){
				curproc->fileTable[i] = of;
				return i;
			}
		}
		//no free slots
		*errp = EMFILE;
	}
	//se qualosa va male devo chiudere
	vfs_close(v);
	return -1;
}


int
sys_close(int fd){
	struct openfile *of = NULL;
	struct vnode *vn;
	int index;

	if(fd <0 || fd>OPEN_MAX) return -1;
	for(int i = 0; i < OPEN_MAX; i++){
		if (curproc->fileTable[i]->fd ==fd){
			of = curproc->fileTable[i];
			index = i;
			break;
		}
	}
	if(of == NULL) return -1;
	curproc->fileTable[index] = NULL;
	vn = of->vn;
	of->vn = NULL;
	if(vn == NULL) return -1;
	vfs_close(vn);
	return 0;
}

//il ritorno di entrambe le funzioni è il numero di byte letti

ssize_t
sys_write (int filehandle, const void *buf, size_t size)
{
	char *b = (char *) buf;

	if(size == 0 || (filehandle != STDOUT_FILENO && filehandle != STDERR_FILENO)){
		return -1;
	}
	
	for (size_t i=0; i< size; i++) {
		putch(b[i]);
	}
	putch('\n');
	return (int) size;
}

ssize_t 	
sys_read (int filehandle, void *buf, size_t size)
{
	char *b = (char *) buf;

	if (size == 0 || filehandle != STDIN_FILENO){
		return -1;
	}
	for (size_t i = 0; i<= size; i++){
		b[i] = getch();

		if (b[i]=='\n' || b[i]=='\r') {
			putch('\n');
			break;
		}
	}
	return (int) size;	
}