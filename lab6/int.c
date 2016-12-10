#define PA 13
#define PB 14
#define PC 15
#define PD 16
#define AX  8

extern int color;
 
int kcinth()
{
   u16    segment, offset;
   int    a,b,c,d, r;

   segment = running->uss; 
   offset = running->usp;

   a = get_word(segment, offset + 2*PA);
   b = get_word(segment, offset + 2*PB);
   c = get_word(segment, offset + 2*PC);
   d = get_word(segment, offset + 2*PD);

   switch(a){
       case 0 : r = running->pid;     break;
       case 1 : r = do_ps();          break;
       case 2 : r = chname(b);        break;
       case 3 : r = kmode();          break;
       case 4 : r = tswitch();        break;
       case 5 : r = do_wait(b);       break;
       case 6 : r = do_exit(b);       break;
        
       case 7 : r = fork();           break;
       case 8 : r = exec(b);          break;
       
       case 90: r = getc();          break;
       case 91: color=running->pid+11;
                r =  putc(b);         break;       
       case 99: do_exit(b);           break;
       default: printf("invalid syscall # : %d\n", a); 
   }
   put_word(r, segment, offset + 2*AX);
}

int kcinth_For_itimer(TQE *t)
{
  char c = '\0';
  printf("input command:");
  c = getc();

     switch(c){
       case '0' : t->action = 0;     break;
       case '1' : t->action = 1;          break;
       case 'a' : t->action = 2;        break;
       case 'b' : t->action = 3;          break;
       case 'c' : t->action = 4;        break;
       case 'd' : t->action = 5;       break;
       case 'e' : t->action = 6;       break;
        
       case 'f' : t->action = 7;           break;
       case 'g' : t->action = 8;          break;
       
       case 'h' : t->action = 90;          break;
       case 'i' : t->action = 91;         break;       
       case 'j' : t->action = 99;           break;
       case 'k' : t->action = 100;         break;       
       case 'l' : t->action = 101;           break;

   
}

int executeAction(int a)
{
     switch(a){
       case 100 : ipid();     break;
       case 101 : ido_ps();          break;
       case 2 : ichname();        break;
       case 3 : ikmode();          break;
       case 4 : itswitch();        break;
       case 5 : ido_wait();       break;
       case 6 : ido_exit1();       break;
        
       case 7 : ifork();           break;
       case 8 : iexec();          break;
       
       case 90: igetc();          break;
       case 91: iputc();         break;       
       case 99: ido_exit2();           break;
   }
}

int fake_syscall(int a, int b, int c, int d)
{
   u16    segment, offset;

   segment = running->uss; 
   offset = running->usp;

   put_word(a, segment, offset + 2*PA);
   put_word(b, segment, offset + 2*PB);
   put_word(c, segment, offset + 2*PC);
   put_word(d, segment, offset + 2*PD);
   kcinth();
}

int ido_ps()
{
  fake_syscall(1,0,0,0);
}

int ikmode()
{
    printf("kmode : enter Kmode via INT 80\n");
    printf("proc %d going K mode ....\n", getpid());
        fake_syscall(3, 0, 0, 0);
    printf("proc %d back from Kernel\n", getpid());
}

int itswitch()
{
    printf("proc %d enter Kernel to switch proc\n", getpid());
        fake_syscall(4,0,0,0);
    printf("proc %d back from Kernel\n", getpid());
}

int ifork()
{
  int child;
  child = fake_syscall(7,0,0,0);
  if (child)
    printf("parent % return form fork, child=%d\n", getpid(), child);
  else
    printf("child %d return from fork, child=%d\n", getpid(), child);
}

int igetc()
{
  fake_syscall(90,0,0,0);
}

int ipid()
{
  fake_syscall(0,0,0,0);
}

int ichname()
{
    char s[64];
    printf("input new name : ");
    gets(s);
    syscall(2, s, 0,0);
}



int ido_wait()
{
    int child, exitValue;
    printf("proc %d enter Kernel to wait for a child to die\n", getpid());
    child = fake_syscall(5, &exitValue, 0, 0);
    printf("proc %d back from wait, dead child=%d", getpid(), child);
    if (child>=0)
        printf("exitValue=%d", exitValue);
    printf("\n");
    return child; 
}



int ido_exit1()
{
   char exitValue;
   printf("enter an exitValue (0-9) : ");
   exitValue=(getc()&0x7F) - '0';
   printf("enter kernel to die with exitValue=%d\n",exitValue);
   fake_syscall(6,exitValue,0,0);
}



int iexec()
{
  int r;
  char filename[32];
  printf("enter exec filename : ");
  gets(filename);
  r = fake_syscall(8,filename,0,0);
  printf("exec failed\n");
}


int iputc()
{
  fake_syscall(91,0,0,0);
}


int ido_exit2()
{
  int r;
  char filename[32];
  printf("enter exec filename : ");
  gets(filename);
  r = fake_syscall(8,filename,0,0);
  printf("exec failed\n");
}

int getpid()
{
  return running->pid;
}
