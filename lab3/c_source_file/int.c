#include "../headers/type.h"

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
        case 7 : r = getc();                         break;
        case 8 : r = putc(b);                        break;
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
