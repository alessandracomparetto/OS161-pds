/*
* LAB2.02
*Tale funzione deve essere in grado quanto meno di attivare la as_destroy() e la chiusura del thread: si suggerisce a tale 
*scopo di realizzare una funzione sys__exit(int status) (creando ad esempio un file kern/syscall/proc_syscalls.c che la contenga). 
*La funzione effettui quindi le chiamate ad as_destroy() e thread_exit() . 
*
*/

#include <types.h>
#include <kern/unistd.h>
#include <stdarg.h>
#include <lib.h>
#include <spl.h>
#include <addrspace.h>
#include <cpu.h>
#include <thread.h>
#include <proc.h>
#include <current.h>
#include <synch.h>
#include <mainbus.h>
#include <vfs.h>          // for vfs_sync()
#include <lamebus/ltrace.h> // for ltrace_stop()
#include <syscall.h>

#include <proc_syscalls.h>

void 
sys__exit(int status){
    struct addrspace* as = proc_getas();
    as_destroy((struct addrspace *)as);
    (void)status;
    thread_exit();   
}