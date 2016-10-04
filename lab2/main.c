/********************* main.c file of MTX4.5 kernel ******************/
#include "headers/type.h"
#include "headers/ext2.h"

procSize = sizeof(PROC);


int init()
{
	PROC *p; 
	int i;
	printf("init ....\n");
	for (i=0; i<NPROC; i++)
	{ 
		// initialize all procs
		p = &proc[i];
		p->pid = i;
		p->status = FREE;
		p->priority = 0;
		p->next = &proc[i+1];
	}
	freeList = &proc[0]; 
	proc[NPROC-1].next = 0; // freeList
	readyQueue = sleepList = 0;
	/**** create P0 as running ******/
	p = get_proc(&freeList); // get PROC from freeList
	p->status = READY;
	running = p;

	nproc = 1;
}

int scheduler()
{
	if (running->status == READY)
	{
		enqueue(&readyQueue, running);
	}

	running = dequeue(&readyQueue);
	rflag = 0;

}

int disp()
{

	printf("\nsuccess\n");
}

main()
{
    printf("MTX starts in main()\n");
    init();      // initialize and create P0 as running
    kfork();     // P0 kfork() P1
    while(1)
    {
    	
	    printf("P0 running\n");

	    if (nproc==2 && proc[1].status != READY)
	    {
			printf("no runable process, system halts\n");
		}
			
	    while(!readyQueue);
	    printf("P0 switch process\n");
	    tswitch();   // P0 switch to run P1

   }
}