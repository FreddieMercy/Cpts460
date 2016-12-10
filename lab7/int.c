/************** syscall routing table ***********/
int kcinth() 
{
  u16 x, y, z, w, r; 
  u16 seg, off;

  seg = running->uss; off = running->usp;

  x = get_word(seg, off+13*2);
  y = get_word(seg, off+14*2);
  z = get_word(seg, off+15*2);
  w = get_word(seg, off+16*2);
  
   switch(x){
       case 0 : r = running->pid;    break;
       case 1 : r = kps();           break;
       case 2 : r = chname(y);       break;
       case 3 : r = kmode();         break;
       case 4 : r = tswitch();       break;
       case 5 : r = kwait();         break;
       case 6 : r = kexit();         break;
       case 7 : r = fork();          break;
       case 8 : r = kexec(y);        break;


       // FOCUS on ksin() nd ksout()
       case 9 : r = ksout(y);        break;
       case 10: r = ksin(y);         break;

       case 99: r = kexit();         break;

       default: printf("invalid syscall # : %d\n", x);

   }
   put_word(r, seg, off+2*AX);
}


int ksin(char *y)
{
  // get a line from serial port 0; write line to Umode

  usgets(stty[0].port, y);
}

int ksout(char *y)
{
  // get line from Umode; write line to serial port 0
  
  uputs(stty[0].port, y);
}

int usgets(int port, char *y)
{  
  // get a line from serial port and write it to y in U space
  struct stty *tty;
  int i = 0;

  for(i = 0; i <  NR_STTY; i++){
    if(stty[i].port == port)
    {
      tty = &stty[i];
      sgetline(tty, y);
      bputc(tty->port, '\n');

      return 1;
    }
  }
  
  return 0;
}

int uputs(int port, char *y)
{
  // output line y in U space to serail port
  struct stty *tty;
  int j = 0;
  int i = 0;
  char c;
  char line[64];

  for(j = 0; j <  NR_STTY; j++){
    if(stty[j].port == port)
    {
      tty = &stty[j];
      c = get_byte(running->uss, y);

        line[i] = c;
        while(c != '\0'){

          c = get_byte(running->uss, y);
          y++;
          i++;
          line[i] = c;
        }

        printf("line = %s\n", line);        
        sputline(tty, line);
        disable_tx(tty);

      return 1;
    }
  }
  
  return 0;
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


int chname(char *name)
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
  // get filename from U space with a length limit of 64 
  while( (*cp++ = get_byte(running->uss, y++)) && length++ < 64 );
  if (!load(filename, segment)); // load filename to segment
  {
    return -1;
  }
  // if load failed, return -1 to Umode
  // re-initialize process ustack for it return to VA=0 
  for (i=1; i<=12; i++)
  {
    put_word(0, segment, -2*i);
  }

  running->usp = -24;
  // new usp = -24
  //-1
  //-2 -3 -4 -5 -6 -7 -8 -9 -10 -11 -12 ustack layout 
  // flag uCS uPC ax bx cx dx bp si di uES uDS
  
  put_word(segment, segment, -2*12);
  // saved uDS=segment
  put_word(segment, segment, -2*11);
  // saved uES=segment
  put_word(segment, segment, -2*2);
  // uCS=segment; uPC=0
  put_word(0x0200, segment, -2*1);
  // Umode flag=0x0200
}

//**************** fork()in MTX5.1 kernel *******************
int copyImage(u16 pseg, u16 cseg, u16 size)
{ 
  u16 i;
  for (i=0; i<size; i++)
  { 
      put_word(get_word(pseg, 2*i), cseg, 2*i);
  }
}



//print name=list contents
printList(char *name, PROC *list)
{
  PROC *tmp = list;
  
  printf("%s: ", name);
  
  if(!list)
  {
    printf("NULL\n");

  }
  else
  {
    while(list->next!=0)
    {
      printf("%d -> ", list->pid);
      //printf("%d (priority = %d) -> ", list->pid, list->priority);
      list = list->next;
    }

    printf("%d -> NULL\n", list->pid);
    //printf("%d (priority = %d) -> ", list->pid, list->priority);

    list = tmp;
  }
}