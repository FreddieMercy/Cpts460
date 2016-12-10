/********************** kernel.c file ***********************/

int body()
{
	char c;

	while(1)
	{

		printList("freelist ", freeList);
		// show freelist
		printList("readyQueue", readyQueue);
		// show readQueue
		printf("proc%d running: priority=%d parent=%d enter a char [s|f|t|c|z|a|p|w|q |  u  ]: ",
		running->pid, running->pri, running->parent->pid );
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
			case 'u' : goUmode();
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


PROC *kfork(char *filename)
{
	int i;
	int segNum;
	PROC *p = get_proc(&freeList);

	if (!p){
		printf("no more PROC, kfork() failed\n");
		return 0;
	}
	p->status = READY;
	p->pri = 1;
	// priority = 1 for all proc except P0
	p->ppid = running->pid;
	p->parent = running;
	/* initialize new proc's kstack[ ] */

	for (i=1; i<10; i++) // saved CPU registers
	{

		p->kstack[SSIZE-i]= 0 ;
	}
	// all 0's
	p->kstack[SSIZE-1] = (int)body; // resume point=address of body()

	//ds and ss

	segNum = (p->pid + 1) << 12;

	p->ksp = &p->kstack[SSIZE-9];

/*

       new PROC
        ------
        |uss | = new PROC's SEGMENT value
        |usp | = -24                                    
        --|---                                    Assume P1 in segment=0x2000:
          |                                              0x30000  
          |                                              HIGH END of segment
  (LOW) | |   ----- by SAVE in int80h ----- | by INT 80  |
  --------|-------------------------------------------------------------
        |uDS|uES| di| si| bp| dx| cx| bx| ax|uPC|uCS|flag|NEXT segment
  ----------------------------------------------------------------------
         -12 -11 -10  -9  -8  -7  -6  -5  -4  -3  -2  -1 |

Use put_word(w, segment, offset) to set the ustack contents to contain:

         uDS=segment, uES=segment,          // MUST be the segment
         di, si, bp, dx, cx, bx, ax = 0     // CPU registers are all 0
         uPC = 0                            // (uCP, uPC) = VRITUAL address 0 
         uCS = segment
         flag=0x0200 (I bit-1 for allow interrupts)
===============================================================================
     }


*/
	if(filename)
	{
	    p->uss = segNum;
	    load(filename, segNum);
	    p->usp = 0x1000-24; //the low end of the stack
		put_word(segNum, segNum, 0x1000-24);
		put_word(segNum, segNum, 0x1000-22);
		put_word(0, segNum, 0x1000-20);
		put_word(0, segNum, 0x1000-18);
		put_word(0, segNum, 0x1000-16);
		put_word(0, segNum, 0x1000-14);
		put_word(0, segNum, 0x1000-12);
		put_word(0, segNum, 0x1000-10);
		put_word(0, segNum, 0x1000-8);
		put_word(0, segNum, 0x1000-6);
		put_word(segNum, segNum, 0x1000-4);
		put_word(0x0200, segNum, 0x1000-2);

	}
	// proc saved sp
	enqueue(&readyQueue, p); // enter p into readyQueue by priority

	return p;
	// return child PROC pointer
}


int do_kfork()
{ 
	PROC *p = kfork(0);
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
}

int chpriority(int pid, int pri)
{

	PROC *p; 
	int i, ok=0, reQ=0;
	if (pid == running->pid)
	{
		running->pri = pri;
		return 1;
	}

	// if not for running, for both READY and SLEEP procs
	for (i=1; i<NPROC; i++)
	{
		p = &proc[i];
		if (p->pid == pid && p->status != FREE)
		{
			p->pri = pri;
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
	pid = getpid();
	printf("input new priority " );
	pri = getpid();
	if (pri<1) pri = 1;
	chpriority(pid, pri);
}