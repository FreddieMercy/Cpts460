/********************** kernel.c file ***********************/
#include "../headers/type.h"
#include "../headers/ext2.h"

int body()
{
	char c;

	while(1)
	{
		printf("nproc = %d\n",nproc);
		if (rflag)
		{
			printf("proc %d: reschedule\n", running->pid);
			rflag = 0;
			tswitch();
		}
		printList("freelist ", freeList);
		// show freelist
		printQ("readyQueue", readyQueue);
		// show readQueue
		printf("proc%d running: priority=%d parent=%d enter a char [s|f|t|c|z|a|p|w|q]: ",
		running->pid, running->priority, running->parent->pid );
		c = getc(); 
		printf("%c\n", c);

		switch(c)
		{
			case 's' : do_tswitch();
			break;
			case 'f' : do_kfork();
			break;
			case 'q' : do_exit();
			break;
			case 't' : do_stop();
			break;
			case 'c' : do_continue();
			break;
			case 'z' : do_sleep();
			break;
			case 'a' : do_wakeup();
			break;
			case 'p' : do_chpriority(); 
			break;
			case 'w' : do_wait();
			break;
			default: printf("invalid command\n"); 
			break;
		}

	}
}

int do_tswitch()
{
  printf("proc %d tswitch()\n", running->pid);
  tswitch();
  printf("proc %d resumes\n", running->pid);
}

PROC *kfork() // create a child process, begin from body()
{
	int i;
	unsigned int segNum;
	PROC *p = get_proc(&freeList);

	if (!p){
		printf("no more PROC, kfork() failed\n");
		return 0;
	}
	p->status = READY;
	p->priority = 1;
	// priority = 1 for all proc except P0
	p->ppid = running->pid;
	// parent = running
	/* initialize new proc's kstack[ ] */

	for (i=1; i<10; i++) // saved CPU registers
	{

		p->kstack[SSIZE-i]= 0 ;
	}
	// all 0's
	p->kstack[SSIZE-1] = (int)body; // resume point=address of body()

	//ds and ss

	segNum = 0x1000;//p->pid << 12;

	p->kstack[SSIZE-10] = segNum;
	p->kstack[SSIZE-11] = segNum;

	//printf("segNum = %x\n", segNum);

	//point to -11 b/c ds and ss,
	//otherwise	
	//p->ksp = &p->kstack[SSIZE-9];

	p->ksp = &p->kstack[SSIZE-11];

	// proc saved sp
	enqueue(&readyQueue, p); // enter p into readyQueue by priority

	nproc++;
	return p;
	// return child PROC pointer

}

int do_kfork()
{ 
	PROC *p = kfork();
	if (p == 0){ printf("kfork failed\n"); return -1; }
	printf("PROC %d kfork a child %d\n", running->pid, p->pid);
	return p->pid;
}

int do_exit()
{ 
	kexit(0);
}

int do_stop()
{ 
	// same as in MTX4.4 
}

int do_continue()
{ 
	// same as in MTX4.4 
}

int do_sleep()
{
	//char c;
	char* s;
	int value;
	
	printf("enter an event value:");
	s = gets(s);
	value = atoi(s);
	//c = getc();
	//printf("you entered: %c\n", c);
	//value = myCharToInt(c);
	printf("proc %d going to sleep on event value: %d\n", running->pid, value);
	ksleep(value);
}

int do_wakeup()
{
	//char c;
	char* s;
	int value;
	
	printf("enter an event value:");
	s = gets(s);
	value = atoi(s);
	//c = getc();
	//printf("you entered: %c\n", c);
	//value = myCharToInt(c);
	printf("attempting to wake up sleeping procs on: %d\n", value);
	kwakeup(value);
}

// call upon the kwait function.
int do_wait()
{
	int pid;
	int status;
	
	printf("waiting..\n");
	
	pid = kwait(&status);
	printf("pid %d, status %d \n", pid, status);
}

// added scheduling functions in MTX4.5
int reschedule()
{
	PROC *p, *tempQ = 0;
	while ( (p=dequeue(&readyQueue)) )
	{ 
		// reorder readyQueue
		enqueue(&tempQ, p);
	}

	readyQueue = tempQ;
	rflag = 0;
	// global reschedule flag
	if (running->priority < readyQueue->priority)
	{
		rflag = 1;
	}
}

int chpriority(int pid, int pri)
{

	PROC *p; 
	int i, ok=0, reQ=0;
	if (pid == running->pid)
	{
		running->priority = pri;
		if (pri < readyQueue->priority)
		{
			rflag = 1;
		}

		return 1;
	}

	// if not for running, for both READY and SLEEP procs
	for (i=1; i<NPROC; i++)
	{
		p = &proc[i];
		if (p->pid == pid && p->status != FREE)
		{
			p->priority = pri;
			ok = 1;
			if (p->status == READY) // in readyQueue==> redo readyQueue
			{
				reQ = 1;
			}
		}
	}

	if (!ok)
	{
		printf("chpriority failed\n");

		return -1;
	}

	if (reQ)
	{
		reschedule(p);
	}

}

int do_chpriority()
{
	int pid, pri;
	printf("input pid " );
	pid = geti();
	printf("input new priority " );
	pri = geti();
	if (pri<1) pri = 1;
	chpriority(pid, pri);
}

