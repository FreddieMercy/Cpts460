#include "../headers/type.h"
#include "../headers/header.h"
/*************************************************************************
  usp  1   2   3   4   5   6   7   8   9  10   11   12    13  14  15  16
----------------------------------------------------------------------------
 |uds|ues|udi|usi|ubp|udx|ucx|ubx|uax|upc|ucs|uflag|retPC| a | b | c | d |
----------------------------------------------------------------------------
***************************************************************************/

/****************** syscall handler in C ***************************/
int kcinth()
{
    u16 segment, offset; 
    int a, b, c, d, r;
    segment = running->uss; 
    offset = running->usp;
    /* get syscall parameters from ustack */
    a = get_word(segment, offset + 2*PA);
    b = get_word(segment, offset + 2*(PA+1));
    c = get_word(segment, offset + 2*(PA+2));
    d = get_word(segment, offset + 2*(PA+3));

    /* route syscall call to kernel functions by call# a */
    switch(a){
        case 0 : r = kgetpid();                      break;
        case 1 : r = kps();                          break;
        case 2 : r = kchname(b);                     break;
        case 3 : r = kkfork();                       break;
        case 4 : r = kkswitch();                     break;
        case 5 : r = kkwait(b);                      break;
        case 6 : kkexit(b);                          break;

        case 7 : fork();                             break;
        case 8 : r = kexec(b);                       break;
        case 9 : khop();                             break;
 	      //case 999: MycheckReg();                        break;
        //case 7 : r = getc();                         break;
        //case 8 : r = putc(b);                        break;
        default: printf("invalid syscall %d\n", a);  break;
    }

    put_word(r, segment, offset + 2*AX); // return value in uax
}
/*
int MycheckReg()
{
      PROC *p = running;
      p->usp = 0x1000-24; //the low end of the stack
      printf("aDS : %x\n", get_word(p->uss, p->usp));
      printf("ES : %x\n", get_word(p->uss, p->usp + 2));
      printf("di : %x\n", get_word(p->uss, p->usp + 4));
      printf("si : %x\n", get_word(p->uss, p->usp + 6));
      printf("bp : %x\n", get_word(p->uss, p->usp + 8));
      printf("dx : %x\n", get_word(p->uss, p->usp + 10));
      printf("cx : %x\n", get_word(p->uss, p->usp + 12));
      printf("bx : %x\n", get_word(p->uss, p->usp + 14));
      printf("ax : %x\n", get_word(p->uss, p->usp + 16));
      printf("PC : %x\n", get_word(p->uss, p->usp + 18));
      printf("CS : %x\n", get_word(p->uss, p->usp + 20));
      printf("flag : %x\n", get_word(p->uss, p->usp + 22));
}
*/

int khop()
{
  int i = 0;
  int segment, segNum;
  int isFree = 0;
  PROC *p1 = &proc[1];
  PROC *tmp = freeList;
  PROC *p;
  char *c;

  if(running->pid != 1)
  {
    printf("only P1 is a Casanova process, which hops\n");
    return;
  }

  printf("To where? : \n");

  for(i = 2; i < 9; i++)
  {
    printf("input %d to choose 0x%d000\n", i, i + 1);
    
  }

  gets(c);

  printf("input = %s\n", c);

  switch(c[0])
  {

    case '2':
      segNum = 2;
      break;
    case '3':
      segNum = 3;
      break;
    case '4':
      segNum = 4;
      break;
    case '5':
      segNum = 5;
      break;
    case '6':
      segNum = 6;
      break;
    case '7':
      segNum = 7;
      break;
    case '8':
      segNum = 8;
      break;
    default:
      printf("ERROR! Not changing anything\n");
      return;
      break;
  }

  p = &proc[segNum];

  while(tmp!=0)
  {
    printf("Checking Proc %d\n",tmp->pid);
    if(tmp==p)
    {
      isFree = 1;
      break;
    }
    tmp = tmp->next;
  }

  if(isFree == 0)
  {
      printf("This Proc had been used, cannot hop! Please quit the process in Proc %d first and then try again\n", segNum);
      return;
  }

  segment = (segNum + 1)<<12;

  p->status = READY;
  p->priority = p1->priority;
  // priority = 1 for all proc except P0
  p->ppid = p1->ppid;
  p->parent = p1->parent;
  /* initialize new proc's kstack[ ] */

  for (i=1; i<10; i++) // saved CPU registers
  {

    p->kstack[SSIZE-i]= p1->kstack[SSIZE-i] ;
  }

  p->kstack[SSIZE-1] = p1->kstack[SSIZE-1]; // resume point=address of body()

  p->ksp = &p->kstack[SSIZE-9];

  copyImage(p1->uss, segment, 32*1024); // copy 32K words

  //setup chosen
  p->uss = segment;
  p->usp = running->usp;
  put_word(segment, segment, p->usp);
  put_word(segment, segment, p->usp+2);
  put_word(segment, segment, p->usp+2*10);

  printf("first word size : %d\n", sizeof(get_word(p1->uss, p1->usp+4)));
  printf("second word size : %d\n", sizeof(get_word(p1->uss, p1->usp+6)));
  //copy values form p1 to chosen
  /*
  put_word(get_word(p1->uss, p1->usp+4), segNum, 0x1000-20);
  put_word(get_word(p1->uss, p1->usp+6), segNum, 0x1000-18);
  put_word(get_word(p1->uss, p1->usp+8), segNum, 0x1000-16);
  put_word(get_word(p1->uss, p1->usp+10), segNum, 0x1000-14);
  put_word(get_word(p1->uss, p1->usp+12), segNum, 0x1000-12);
  put_word(get_word(p1->uss, p1->usp+14), segNum, 0x1000-10);
  put_word(get_word(p1->uss, p1->usp+16), segNum, 0x1000-8);
  put_word(get_word(p1->uss, p1->usp+18), segNum, 0x1000-6);

  put_word(get_word(p1->uss, p1->usp+22), segNum, 0x1000-2);

  */
  
/*
  if(tmp != 0)
  {
    tmp->next = p->next;
  }

  if(p1->next != p)
  {
    p->next = p1->next;
    p1->next = p;
  }

  printf("running->next pid = %d\n", p->pid);
  */
  
  if (running->status == READY)
  {
    enqueue(&readyQueue, running);
  }

  running = p;
  color = running->pid + 0x0A;
  //do_tswitch();
}


//============= WRITE C CODE FOR syscall functions ======================

 // WRITE YOUR C code

int kgetpid()
{
   if(running == 0)
   {
      printf("I am NULL... I am super weird~~\n");
      return -1;
   }

   printf("I am proc %d in U mode, running segment = %x\n", running->pid, running->pid << 12);

   return running->pid;
}



int kps()
{
    //WRITE C code to print PROC information
    PROC *p;
    int i = 0;

    char status[7];

    printf("name         status         pid         ppid\n");

    for(i=0;i<NPROC;i++)
    {    
        p = &proc[i];
        findStat(p->status, status);

        if(p->status == FREE)
        {
            printf("%s         %s         \n", p->name, status);
        }
        else
        {
            printf("%s         %s         %d         %d\n", p->name, status, p->pid, p->ppid);
        }

        cleanCharArray(status);
    }
}


int kchname(char *name)
{
  char c = 'a';
  int i =0;

  while(c)
  {
    //name is used as the offset here think of its address value
    c = get_byte(running->uss, name + i);
    running->name[i] = c;
    i++;
  }
}
int kkfork()
{
  /*
  use you kfork() in kernel;
  return child pid or -1 to Umode!!!
  */

  PROC *p = kfork(file);
  if (p == 0){ 
      printf("kfork failed\n"); 
      put_word(-1, running-> uss, running-> usp + 8*AX);
      return -1; 
    }
  printf("PROC %d kfork a child %d\n", running->pid, p->pid);
  return p->pid;
}

int kkswitch()
{
    return tswitch();
}

int kkwait(int *status)
{

/*
  use YOUR kwait() in LAB3;
  return values to Umode!!!
  */

  put_word(kwait(status), running-> uss, running-> usp + 8*AX);

  
}

int kkexit(int value)
{
  kexit(value);
  /*
    use your kexit() in LAB3
    do NOT let P1 die
    */


}


findStat(int num, char *status)
{

    switch(num)
    {
        case FREE:
            strcpy(status, "FREE");
            break;
        case ZOMBIE:
            strcpy(status, "ZOMBIE");
            break;
        case READY:
            strcpy(status, "READY");
            break;
        case RUNNING:
            strcpy(status, "RUNNING");
            break;

        case STOPPED:
            strcpy(status, "STOPPED");
            break;
        case SLEEP:
            strcpy(status, "SLEEP");
            break;
        default:
            strcpy(status, "DIED");
            break;
    }


}

cleanCharArray(char *status)
{

    int i;

    for(i = strlen(status)-1; i >= 0; i--)
    {
        status[i]='\0';
    }

}

int kexec(char *y)
// y points at filenmae in Umode space
{
  int i, length = 0;
  char filename[64], *cp = filename;
  u16 segment = running->uss;
  // same segment
  /* get filename from U space with a length limit of 64 */
  while( (*cp++ = get_byte(running->uss, y++)) && length++ < 64 );
  if (!load(filename, segment)); // load filename to segment
  {
    return -1;
  }
  // if load failed, return -1 to Umode
  /* re-initialize process ustack for it return to VA=0 */
  for (i=1; i<=12; i++)
  {
    put_word(0, segment, -2*i);
  }

  running->usp = -24;
  // new usp = -24
  /* -1
  -2 -3 -4 -5 -6 -7 -8 -9 -10 -11 -12 ustack layout */
  /* flag uCS uPC ax bx cx dx bp si di uES uDS
  */
  put_word(segment, segment, -2*12);
  // saved uDS=segment
  put_word(segment, segment, -2*11);
  // saved uES=segment
  put_word(segment, segment, -2*2);
  // uCS=segment; uPC=0
  put_word(0x0200, segment, -2*1);
  // Umode flag=0x0200
}

/**************** fork()in MTX5.1 kernel *******************/
int copyImage(u16 pseg, u16 cseg, u16 size)
{ 
  u16 i;
  for (i=0; i<size; i++)
  { 
      put_word(get_word(pseg, 2*i), cseg, 2*i);
  }
}

int fork()
{ 

  int pid; 
  u16 segment;
  PROC *p = kfork(0);
  // kfork() a child, do not load image file
  if (p==0) 
  {
    return -1;
  }
  // kfork failed
  segment = (p->pid+1)*0x1000; // child segment
  copyImage(running->uss, segment, 32*1024); // copy 32K words
  p->uss = segment;
  // child’s own segment
  p->usp = running->usp;
  // same as parent's usp
  //*** change uDS, uES, uCS, AX in child's ustack ****
  put_word(segment, segment, p->usp);
  // uDS=segment
  put_word(segment, segment, p->usp+2);
  // uES=segment
  put_word(0, segment, p->usp+2*8);
  // uax=0
  put_word(segment, segment, p->usp+2*10);
  // uCS=segment
  return p->pid;
}
