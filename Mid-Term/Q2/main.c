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
    int i;
    printf("init ....\n");
    for (i=0; i<NPROC; i++)
    { 
      // initialize all procs
      p = &proc[i];
      p->pid = i;
      p->status = FREE;
      p->priority = 0;
      strcpy(proc[i].name, pname[i]);
      p->next = &proc[i+1];
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
     put_word(0x1000, 0, (vector*4) + 2);
}

main()
{
    file = "/bin/u1";
    init();      // initialize and create P0 as running
    //printf("proSize : %d\n", procSize);
    set_vector(80, int80h);

    kfork(file);     // P0 kfork() P1

    //checkWord();

    while(1)
    {
      
      printf("P0 running\n");
      while(!readyQueue);
      printf("P0 switch process\n");
      tswitch();   // P0 switch to run P1

   }
}

    
checkWord()
{
  int i = 1;
  int size;
  running->usp = running->uss;

  while(running->uss + segSize != running->usp)
  {
    size = sizeof(get_word(running->uss, running->usp));
    printf("%dst word size : %d\n", i, size);
    i++;
    running->usp+=size;

  }
}

disp()
{
  printf("success\n");
}