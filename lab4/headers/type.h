
/*************** type.h file *****************/
#ifndef TYPE_H
#define TYPE_H

#define NPROC        9
#define SSIZE     1024
#define FREE         0
#define READY        1
#define RUNNING      2
// for clarity only, not needed or used
#define STOPPED      3
#define SLEEP        4
#define ZOMBIE       5

#define AX           8
#define PA          13
#define MAXLEN    1024

#define READ_PIPE    2
#define WRITE_PIPE   3

#define PSIZE       10
#define NPIPE       20
#define NFD         20

#define BROKEN_PIPE 99

typedef unsigned short u16;
typedef unsigned long u32;
typedef unsigned char u8;


typedef struct pipe{
    char buf[PSIZE];
    int head, tail;
    int data, room;
    int nreader, nwriter;
}PIPE;

typedef struct oft{
    int mode;
    int refCount;
    struct pipe *pipe_ptr;
}OFT;

//dont move around ksp next or kstack
typedef struct proc{
    struct proc *next;
    int    *ksp;               // at offset 2 //kernal stack space 

    int    uss;                // user stack space // what segment the users stack is in
    int    usp;                // user stack pointer

    int    inkmode;            // at offset 8 // in k mode

    int    pid;                // add pid for identify the proc
    int    status;             // status = FREE|READY|RUNNING|SLEEP|ZOMBIE    
    int    ppid;               // parent pid
    struct proc *parent;
    int    priority;
    int    event;
    int    exitCode;
    char   name[32];           // name string of PROC

    int    kstack[SSIZE];      // per proc stack area

    OFT    *fd[NFD];
}PROC;

char *file;
//Assembly Functions
//Globals ************
PROC *running;              //current executing PROC
PROC *freeList;             //list of FREE PROCs
PROC *readyQueue;           //priority queue of READY PROCs
PROC *sleepQueue;
PROC proc[NPROC];

int pd[2], n, i;
char line[256], *s="data from pipe ";

#endif
