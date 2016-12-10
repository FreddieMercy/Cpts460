#include "../headers/type.h"

char *cmd[]={"getpid", "ps", "chname", "kfork", "switch", "wait", "exit", "fork", "exec", "abc", "getc", "putc", "abc", "pipe", "read_pipe", "write_pipe", "close_pipe", "pfd", 0};
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
/*
int upipe_demo()
{
  return syscall(12,0,0);
}
*/
int upipe()
{
  return syscall(30,0,0,0);
}
int uread_pipe()
{
  char *s, *c;
  char *line = malloc(sizeof(char)*256);
  printf("input fd:\n");
  gets(s);
  printf("You entered : %d\n", atoi(s));

  printf("number of bytes: \n");
  gets(c);
  printf("You entered : %d\n", atoi(c));

  return syscall(31,atoi(s), line,atoi(c));
}
int uwrite_pipe()
{
  
  char *s, *c;
  printf("input fd:\n");
  gets(s);
  printf("You entered : %d\n", atoi(s));

  printf("content to write: \n");
  gets(c);
  printf("You entered : %s\n", c);
  
  return syscall(32,atoi(s),c,strlen(c));
  
}
int uclose_pipe()
{
  
  char *s;
  gets(s);
  return syscall(33,atoi(s),0,0);
  
}
int upfd()
{
  return syscall(34,0,0,0);
}

int ucheckReg()
{
  syscall(999,0,0);
}

int ugetc()
{
  return syscall(10,0,0,0);
}

int uputc()
{
  char c = '\0';
  printf("input char : ");
  getc(c);
  return syscall(11,c,0,0);
}

int find_cmd(char *name)
{
  int i;

  //numCmd = sizeof(cmd)/2;

  for(i = 0; i < numCmd - 1; i++)
  {
    if(!strcmp(name, cmd[i]))
    {
      return i;
    } 
  }

  return -1;
}

int hop()
{
  return syscall(9,0,0);
}

int fork()
{ 
    return syscall(7,0,0,0); 
}


int exec(char *s)
{ 
    return syscall(8,s,0,0); 
}

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

int ufork()
{
// user fork command
  int child;
  //printf("from ufork\n");
  child = fork();
  //printf("Something\n");
  (child)? printf("parent ") : printf("child ");
  printf("%d return form fork, child_pid=%d\n", getpid(), child);
}

int uexec()
{
  int r; 
  char filename[64];
  printf("enter exec filename : ");
  gets(filename);
  r = exec(filename);
  printf("exec failed\n");
}

int atoi(char *str)
{
  int retVal = 0;
  int sign = 1;
  char *strPtr = str;

  if(*strPtr == '-')
  {
    sign = -1;
    strPtr++;
  }
  //go untill the end of the string
  while(*strPtr != 0)
  {
    retVal *= 10;
    retVal += (int)*strPtr - '0';
    
    strPtr++;
  }

  return retVal * sign;
}