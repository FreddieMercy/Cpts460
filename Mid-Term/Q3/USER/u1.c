#include "ucode.c"
int color;

main()
{ 
  char name[64]; int pid, cmd;

  while(1)
  {
    //ucheckReg();
    pid = getpid();
    color = 0x0C;
       
    printf("----------------------------------------------\n");
    printf("I am proc %d in U mode: running segment=%x\n",getpid(), getcs());
    show_menu();
    printf("Command ? ");
    gets(name); 
    if (name[0]==0) 
        continue;

    cmd = find_cmd(name);
    switch(cmd){
           case 0 : getpid();           break;
           case 1 : ps();               break;
           case 2 : chname();           break;
           case 3 : kfork();            break;
           case 4 : kswitch();          break;
           case 5 : wait();             break;
           case 6 : exit();             break;
           case 7 : ufork();            break;
           case 8 : uexec();            break;
           //case 9 : hop();              break;
           case 10: ugetc();            break;
           case 11: uputc();            break;
           //case 12: upipe_demo();       break;
           case 13: upipe();            break;
           case 14: uread_pipe();       break;
           case 15: uwrite_pipe();      break;
           case 16: uclose_pipe();      break;
           case 17: upfd();             break;
           default: invalid(name);      break;
    }
  }
}




