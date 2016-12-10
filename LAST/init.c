
//*************************************************************************
//                      Logic of init.c 
// NOTE: this init.c creates only ONE login process on console=/dev/tty0
// YOUR init.c must also create login processes on serial ports /dev/ttyS0
// and /dev/ttyS1.. 
//************************************************************************

#include "ucode.c"  //<========== AS POSTED on class website

char *console[3] = {"/dev/tty0", "/dev/ttyS0", "/dev/ttyS1"};
char *_console;

main(int argc, char *argv[])
{
    int i=0;

    stdin = open(console[0], 0);
    stdout = open(console[0], 1);

    //for (i = 0; i < 3; i++)
    //{
        _console = console[i];

      //1. // open /dev/tty0 as 0 (READ) and 1 (WRTIE) in order to display messages
        //int in = open("/dev/ttyS1", 0);
        //int out = open("/dev/ttyS1", 1);

      //2. // Now we can use printf, which calls putc(), which writes to stdout
         printf("Freddie's INIT.c : fork a login task on console\n"); 
         child = fork();

         if (child)
         {
            
            //printf("parent is %d\n", getppid());
            //parent();
            _console = console[2];
            child = fork();
            if(child)
            {
              parent();
              _console = console[1];
              child = fork();
              if(child)
              {
                parent();
              }
              else
              {
                login();
              }
            }
            else
            {
              login();
            }
            
         }
             
         else             // login task
         {
            
            //printf("child is %d\n", getpid());
            pid = wait(&status);
            //printf("%d. console = %s\nstdin = %d, stdout = %d\n", i, _console, stdin, stdout);
            //printf("From Main\n");
            login();
         }
             
    //}

/*
    for (i = 0; i < 3; i++ )
    {
        wait(&status);
    } 
    */
}   

int login()
{
  char *cmd = malloc(strlen("login ")+strlen(_console)+1);
  strcpy(cmd, "login ");
  strcat(cmd, _console);
  //exec("login /dev/ttyS1");
  //printf("doing: %s\n", cmd);
  exec(cmd);
}
      
int parent()
{
  while(1){
    //printf("From Parent\n");
    printf("Freddie's INIT.c : waiting .....\n");

    pid = wait(&status);

    if (pid == child)
    {
        printf("Freddie's INIT: forks a new console login\n");
        child = fork();
        if(child)
        {
          //printf("Skip child\n");
          continue;
        }
        else
        {
          //printf("Now we login from Parent\n");
          login();
        }
    }
    else
    {
        printf("Freddie's INIT : buried an orphan child %d\n", pid);
    }
  }
}
