// ucode.c file

char *cmd[]={"getpid", "ps", "chname", "kfork", "switch", "wait", "exit", "getc", "putc", 0};
int numCmd = sizeof(cmd)/2;

#define LEN 64

int show_menu()
{
  int i;
  printf("***************** Menu *******************\n");
  printf("* ");
  for(i = 0; i < numCmd - 1; i++)
  {
    printf("%s, ", cmd[i]);
  }
  printf(" *\n");
  printf("******************************************\n");
}

int find_cmd(char *name)
{
  int i;

  //numCmd = sizeof(cmd)/2;

  for(i = 0; i < numCmd - 1; i++)
  {
    printf("find_cmd: cmd[i] = %s : name = %s \n", cmd[i], name);
    if(!strcmp(name, cmd[i])) return i;
  }

  return -1;
}

int fork()
{ return syscall(7,0,0,0); }


int exec(char *s){ return syscall(8,s,0,0); }

int getpid()
{
   return syscall(0,0,0);
}

int ps()
{
   return syscall(1, 0, 0);
}

int chname()
{
    char s[32];
    printf("input new name : ");
    gets(s);
    return syscall(2, s, 0);
}

int kfork()
{   
  int child, pid;
  pid = getpid();
  printf("proc %d enter kernel to kfork a child\n", pid); 
  child = syscall(3, 0, 0);
  printf("proc %d kforked a child %d\n", pid, child);
}    

int kswitch()
{
    return syscall(4,0,0);
}

int wait()
{
    int child, exitValue;
    printf("proc %d enter Kernel to wait for a child to die\n", getpid());
    child = syscall(5, &exitValue, 0);
    printf("proc %d back from wait, dead child=%d", getpid(), child);
    if (child>=0)
        printf("exitValue=%d", exitValue);
    printf("\n"); 
} 

int geti()
{
  // return an input integer
  char *c;
  int i;
  *c = getc();
  *(c + 1) = 0;
  i = atoi(c);
}

int exit()
{
   int exitValue;
   printf("enter an exitValue: ");
   exitValue = geti();
   printf("exitvalue=%d\n", exitValue);
   printf("enter kernel to die with exitValue=%d\n", exitValue);
   _exit(exitValue);
}

int _exit(int exitValue)
{
  return syscall(6,exitValue,0);
}

int invalid(char *name)
{
    printf("Invalid command : %s\n", name);
}

