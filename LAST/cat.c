
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
    char buff[BLKSIZE];

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

            myread2(infile, 2);

        }
        else
        {
            printf("%s", src);
            //write(1, src, strlen(src));
        }
    }

    else if(argc == 1)
    {
        myread2(infile, 1);

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

int myread2(int infile, int argc)
{
    int byteRead = -1;
    int total = 0;

    char *buff[BLKSIZE];

    do
    {
        //printf("success from copy it\n");
        if(argc == 2)
        {
            byteRead = readByLine(infile, buff, BLKSIZE);
        }
        else
        {
            byteRead = readByLine33(infile, buff, BLKSIZE);
        }

        if(byteRead > 0)
        {
        
            //the buff got from readByLine has '\n' in its end, remove it and display '\n' by printf otherwise it looks weird
            buff[strlen(buff)-1] = '\n';

            if(strlen(buff) > 0)
            {
                printf("%s", buff);
            }
        }

        total += byteRead;
        //printf("byteRead = %d\n", byteRead);

    }while(byteRead != 0);

    return total;
}
