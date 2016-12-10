
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
    char *line = &buff;
    int i = 0;


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

            myread2(infile);

        }
        else
        {
            printf("%s\n", src);
        }
    }

    else if(argc == 1)
    {
        while (1)
        {
            gets(line);
            printf("%s\n",line);
            for(i = 0; i < strlen(buff); i++)
            {
                buff[i] = '\0';
            }
        }
    }

    else
    {
        printf("error\n");
    }

    close(infile);

    return 0;
}

int myread2(int infile)
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

    }while(byteRead != 0);

    return total;
}
