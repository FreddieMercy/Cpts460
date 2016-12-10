#include "headers/type.h"
#include "headers/header.h"

//Functions ************
int body();
int set_vector(u16 segment, u16 handler);

//Assembly Functions **********
int int80h();

int do_tswitch()
{
  tswitch();
}

int init()
{
    PROC *p; 
    int i, j, z=0;

    printf("init ....\n");

    for(i = 0; i < NFD; i++)
    {
      samples[i].status = FREE;
    }

    for (i=0; i<NPROC; i++)
    { 
      // initialize all procs
      p = &proc[i];
      p->pid = i;
      p->status = FREE;
      p->priority = 0;
      strcpy(proc[i].name, pname[i]);
      p->next = &proc[i+1];

      p->fd[0]->status=FREE;
      p->fd[1]->status=FREE;
      p->fd[2]->status=FREE;
      p->fd[3]->status=FREE;
      p->fd[4]->status=FREE;

    }

    freeList = &proc[0]; 
    proc[NPROC-1].next = 0; // freeList
    readyQueue = 0;
    /**** create P0 as running ******/
    p = get_proc(&freeList);
    p->status = RUNNING;
    p->ppid   = 0;
    p->parent = p;
    running = p;



    for(i = 0; i < NFD; i++)
    {
      ofts[i].status = FREE;
    }

    for(i = 0; i < NPIPE; i++)
    {
      pipes[i].status = FREE;
    }

    printf("done\n");
}

int scheduler()
{
  if (running->status == READY)
  {
    enqueue(&readyQueue, running);
  }

  running = dequeue(&readyQueue);
    color = running->pid + 0x0A;

}

int set_vector(u16 vector, u16 handler)
{
     // put_word(word, segment, offset)
     put_word(handler, 0, vector*4);
     put_word(0x1000,  0, (vector*4) + 2);
}

main()
{
    file = "/bin/u1";
    init();      // initialize and create P0 as running
    set_vector(80, int80h);

    kfork(file);     // P0 kfork() P1
    
    while(1)
    {
      
      printf("P0 running\n");
      
      while(!readyQueue);
      printf("P0 switch process\n");
      tswitch();   // P0 switch to run P1

   }
}

    
