
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

    if(argc == 3) // equal 3 for regular, equal 2 for pipe
    {
        src = argv[2];
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

            myread3(infile, argv[1]);

        }
        else
        {
            printf("the file %s does not exist\n", src);
        }
    }
    else if(argc == 2)
    {
        myread3(infile, argv[1]);
    }

    else
    {
        printf("error\n");
    }

    if(infile != 0)
    {
        close(infile);
    }

    //exit(0);

    return 0;
}

int myread3(int infile, char *keyword)
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
                if(checkKeyWord(buff, keyword)) //check contains the keyword
                {
                    printf("%s\n", buff);
                }
            }
        }
        
        total += byteRead;

    }while(byteRead != 0);

    return total;
}

int checkKeyWord(char *line, char *keyword)
{
    return checkKeyWord2(line, keyword, 0, 0, 1);
}

int checkKeyWord2(char *line, char *keyword, int index, int j, int root)
{
    int i = index;
    int contains = 0;

    if(keyword[j] == '\0')
    {
        return 1;
    }

    if(index >= strlen(line))
    {
        //printf("returned lines\n");
        return 0;
    }

    for(i = index; i < strlen(line); i++)
    {
        if(line[i] == keyword[j])
        {
            contains = checkKeyWord2(line, keyword, i+1, j+1, 0);
        }
        else
        {
            if(!root)
            {
                return 0;
            }

            contains = 0;
        }

        if(contains)
        {
            break;
        }
    }

    return contains;

}