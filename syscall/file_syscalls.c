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

#include "../include/file_syscalls.h" 

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