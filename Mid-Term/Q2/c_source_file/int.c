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
        case 999: MycheckReg();                        break;
        //case 7 : r = getc();                         break;
        //case 8 : r = putc(b);                        break;
        default: printf("invalid syscall %d\n", a);  break;
    }

    put_word(r, segment, offset + 2*AX); // return value in uax
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

   printf("I am proc %d in U mode, running segment = %x\n", running->pid, 0x1000+running->pid*0x800);

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
  segment = 0x1000+p->pid*segSize; // child segment
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

int MycheckReg()
{
      PROC *p = running;
        p->usp = segSize-24; //the low end of the stack
      printf("DS : %x\n", get_word(p->uss, p->usp));
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