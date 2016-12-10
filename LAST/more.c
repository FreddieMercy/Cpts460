
#include "ucode.c"
#include "imploy.c"

int main(int argc, char* argv[])
{
    STAT sr;
    char *src;
    int sfd;
    int infile = 0;
    char *tmp;
    char mybuff[BLKSIZE];

    int i = 0;
    int bytes = 0;
    char *buff[BLKSIZE];
    char c ='\0';
    char *tty[64];

/*
    int i;
        for (i=0; i<argc; i++){
        printf("argv[%d]=%s\n", i, argv[i]);
      }
*/

    //printf("in = %d\nout = %d\n", in, out);

    if(argc == 2)
    {
        src = argv[1];
        sfd = stat(src, &sr);

        if(sfd >= 0)
        {
            if(S_ISLNK(sr.st_mode))
            {
                 readlink(src, mybuff);

                 tmp=&mybuff;
                 //printf("mybuff : %s\n", tmp);

                 infile = open(tmp, 0);
            }
            else
            {
                infile = open(src, 0);
            }

            //--------------------------------------------
            myreadForCharandBreak(infile);

            //------------------------------------------------
        }
        else
        {
            printf("cannot find %s\n", src);

            return 0;
        }
    }

    else if(argc == 1)
    {
        
        infile = dup(0);
        close(0);
        gettty(tty);
        open(tty, O_RDONLY);
        myreadForCharandBreak(infile);
        /*

        while(c != 'q' && bytes > 0)
        {
            c = '\0';

            c = getc();

            if(strlen(c) == 33) //\n
            {
                //printf("more 1\n");
                bytes = myreadForMore(infile, 1);

                for(i = 0; i < strlen(buff); i++)
                {
                    buff[i] = '\0';
                    //printf("more 1\n");
                }
            }

            else
            {
                bytes = read(infile, buff, 1);
                //printf("more 2\n");

                if(buff[0] != '\n')
                {
                    printf("%s", buff);
                }
                else
                {
                    printf("\n");
                }
            }
        


            //printf("bytes = %d\n", bytes);

        }


        printf("\n");

        */
    }

    else
    {
        printf("error\n");
    }

    if(infile != 0)
    {
        close(infile);
    }

    return 0;
}

int myreadForMore(int infile, int lines)
{
    int byteRead = -1;
    int total = 0;

    char *buff[BLKSIZE];

    do
    {
        //printf("success from copy it\n");
        byteRead = readByLine(infile, buff, BLKSIZE);

        if(byteRead > 0)
        {
        
            //the buff got from readByLine has '\n' in its end, remove it and display '\n' by printf otherwise it looks weird
            buff[byteRead-1] = '\0';

            if(strlen(buff) > 0)
            {
                printf("%s\n", buff);
            }
        }

        total += byteRead;
        lines--;

        //printf("byteRead = %d\n", byteRead);

    }while(byteRead != 0 && lines > 0);

    //printf("done more\n");

    return total;
}


int myreadForCharandBreak(int infile)
{
    int i = 0;
    int bytes = 0;
    char *buff[BLKSIZE];
    int isRoot = 1;
    char c[1];

    c[0] = '\0';
    bytes = myreadForMore(infile,19);

    //printf("from more: infile == %d\n", infile);

    //printf("from more: bytes % BLKSIZE == %d\n", bytes % BLKSIZE);

    do
    {
        c[0] = '\0';

        if(!isRoot)
        {
            //printf("it is root\n");
            
            read(0, c, 1);
            
        }
        else
        {
            //printf("now it is not root\n");
            isRoot = 0;
        }

        if(strlen(c[0]) == 33) //\n
        {
            //printf("more 1\n");
            bytes = myreadForMore(infile, 1);

            for(i = 0; i < strlen(buff); i++)
            {
                buff[i] = '\0';
                //printf("more 1\n");
            }
        }

        else
        {
            bytes = read(infile, buff, 1);
            //printf("more 2\n");

            if(buff[0] != '\n')
            {
                printf("%s", buff);
            }
            else
            {
                printf("\n");
            }
        }

        //printf("bytes = %d\n", bytes);

    }while(c[0] != 'q' && bytes > 0);


    printf("\n");

    //printf("exiting\n");

    //exit(0);
}