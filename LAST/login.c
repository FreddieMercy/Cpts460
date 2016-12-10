
//***********************************************************************
//                   LOGIC of login.c file
//***********************************************************************

#include "ucode.c"
#include "imploy.c"


main(int argc, char *argv[])   // invoked by exec("login /dev/ttyxx")
{
  int fd;
  char buff[256];
  char buff2[16];
  //char *s1, *s2;

  //printf("logging in via %s\n", argv[1]);

  tty =  argv[1];

  close(0); 
  close(1); 
  close(2); // login process may run on different terms
//  close(3);
//  close(4);
//  close(5);

  // open its own tty as stdin, stdout, stderr
  in = open(argv[1], 0);
  out = open(argv[1], 1);
  err = open(argv[1], 2);

  printf("***\nlogin:\nin = %d, out = %d, err = %d\n***\n", in, out, err);

  //printf("argv = %s\n", argv[1]);

  settty(tty);   // store tty string in PROC.tty[] for putc()
  //printf("tty = %s\n", tty);

  // NOW we can use printf, which calls putc() to our tty
  printf("Freddie's LOGIN : open %s as stdin, stdout, stderr\n", tty);

  signal(2,1);  // ignore Control-C interrupts so that 
                // Control-C KILLs other procs on this tty but not the main sh

  while(1){

      fd = open("/etc/passwd", 0);
      printf("Freddie's login: ");
      gets(buff);

      printf("Freddie's password: ");
      gets(buff2);

      //s1 = &buff;
      //s2 = &buff2;

      //printf("%s\n", s1);
      //printf("%s\n", s2);

/*
    5. verify user name and passwd from /etc/passwd file

    6. if (user account valid){
          setuid to user uid.
          chdir to user HOME directory.
          exec to the program in users's account
       }
       */

      if(parsingTheString(fd, &buff, &buff2) == 1)
      {
          //printf("success\n");
          //printf("uid = %d, gid = %d, dir = %s, sh = %s\n", uid, gid, dir, sh);
          chuid(uid,gid);
          chdir(dir);
          //??
          close(fd);
          exec(sh);
      }
      else
      {
        close(fd);
        printf("Fredddie login failed, try again\n");
      }
  }
}
