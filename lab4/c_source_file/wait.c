#include "../headers/type.h"

/******************* wait.c file ****************************/

int ksleep(int event)
{
	running->event = event; // record event in PROC.event
	running->status = SLEEP; // change status to SLEEP
	tswitch();
	// give up CPU
}

int kwakeup(int event)
{
	int i; 
	PROC *p;
	for (i=1; i<NPROC; i++)
	{
		// not applicable to P0
		p = &proc[i];
		if (p->status == SLEEP && p->event == event)
		{
			p->event = 0;
			// cancel PROC’s event
			p->status = READY; // make it ready to run again
			enqueue(&readyQueue, p);
		}
	}
}

int ready(PROC *p) 
{ 
	p->status=READY; 
	enqueue(&readyQueue, p); 
}

int kexit(int exitValue)
{
	/*

1. erase process user-mode context, e.g. close file descriptors,
release resources, deallocate user-mode image memory, etc.
2. dispose of children processes, if any.
3. record exitValue in PROC.exitCode for parent to get.
4. become a ZOMBIE (but do not free the PROC)
5. wakeup parent and, if needed, also the INIT process P1.

	*/

	int i, wakeupP1 = 0;
	if (running->pid==1 /*&& nproc>2*/)
	{ // nproc = number of active PROCs
		printf("other procs still exist, P1 can't die yet\n");
		return -1;
	}

	/* send children (dead or alive) to P1's orphanage */

	for (i = 1; i < NPROC; i++)
	{
		PROC *p = &proc[i];
		if (p->status != FREE && p->ppid == running->pid)
		{
			p->ppid = 1;
			p->parent = &proc[1];
			wakeupP1++;
		}
	}

	/* record exitValue and become a ZOMBIE */
	running->exitCode = exitValue;
	running->status = ZOMBIE;
	/* wakeup parent and also P1 if necessary */
	kwakeup(running->parent); 
	// parent sleeps on its PROC address
	if (wakeupP1)
	{
		kwakeup(&proc[1]);
	}

	printf("proc %d exited, extValue is %d\n", running->pid, running->exitCode);

	tswitch();
	// give up CPU

}

int kwait(int *status) // wait for ZOMBIE child
{
	/*
if (caller has no child)
	return -1 for error;
while(1){ // caller has children

	search for a (any) ZOMBIE child;

	if (found a ZOMBIE child){
		get ZOMBIE child pid
		copy ZOMBIE child exitCode to *status;
		bury the ZOMBIE child (put its PROC back to freeList)
		return ZOMBIE child pid;
	}
	//**** has children but none dead yet ***
	ksleep(running); // sleep on its PROC address
}
	*/

	PROC *p; 
	int i, hasChild = 0;
	while(1)
	{
		// search PROCs for a child
		for (i=1; i<NPROC; i++)
		{
			// exclude P0
			p = &proc[i];
			if (p->status != FREE && p->ppid == running->pid)
			{
				hasChild = 1;
				// has child flag
				if (p->status == ZOMBIE)
				{
					// lay the dead child to rest
					*status = p->exitCode;
					// collect its exitCode
					p->status = FREE;
					// free its PROC
					put_proc(&freeList, p); // to freeList
			
					// once less processes
					return(p->pid);
				// return its pid
				}
			}
		}

		if (!hasChild)
		{
			return -1;
		}
		
		// no child, return ERROR
		ksleep(running); // still has kids alive: sleep on PROC address
	}
}

/*
int ksleep(int event)
{
	running->event = event;
	running->status = SLEEP;
	enqueue(&sleepList, running);
	tswitch();
}
*/

/*
int kwakeup(int event)
{
	int i = 0;
	PROC *p = sleepList;

	if (p == 0)
	{
		printf("SleepList is empty\n");
		
		return -1;
	}
	
	// check the entire sleepList for any PROC with matching (event == p->event)
	while(p)	
	{
		// found one matching the conditions. To wake up, set its status to READY.
		if (p->status == SLEEP && p->event == event)
		{
			p->event = 0;
			p->status = READY;
			
			// if the dequeue operation from the sleepList finds the correct PROC.. 
			if(dequeue(&sleepList, p))	
			{
				enqueue(&readyQueue, p);	// put it into readyQueue again.
				i++;
			}
		}
		
		else
		{
			p = p->next;
		}
	}
	
	if(!i)
	{
		printf("No items in the sleepList with event %d \n", event);
	}
}
*/