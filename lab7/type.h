typedef unsigned char  u8;
typedef unsigned short u16;
typedef unsigned long  u32;

#define NULL      0
#define NPROC     9
#define SSIZE   512

#define FREE         0
#define READY        1
#define RUNNING      2
#define STOPPED      3
#define SLEEP        4
#define ZOMBIE       5
#define BLOCK     4
#define AX        8
#define BEEP    0x7



typedef struct proc{
        struct proc *next;
        int    *ksp; 
        int    uss,usp;
        int    inkmode;

        int     pid;
        int     ppid;
        struct proc *parent;
        int     status;
        int     pri;          
        char    name[16];    
        int     event;
        int     exitCode;
        int     size;        
  int kstack[SSIZE];
}PROC;        

struct semaphore{
  int value;
  PROC *queue;
};