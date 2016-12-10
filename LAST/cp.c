
#include "ucode.c"

char *dirname(char *s)
{
    char *path = s;

    char dir[256];
    int i = 0;
    //printf("dir main: %s\n", s);

    while (*path)
    {

        path++;
    }

    while (*(path) != '/' && path != s)
    {
        path--;
    }

    if (path == s) 
    {
        if (*s == '/')
        {
            dir[0] = '/';
            dir[1] = '\0';
            return &dir;
        }
        else
        {
            dir[0] = '.';
            dir[1] = '\0';
            return &dir;
        }
    }

    while(s != path)
    {
        dir[i] = *s;
        s++;
        i++;
    }

    dir[i] = '\0';

    while(i != 0)
    {
        s--;
        i--;
    }

    return &dir;
}

char *basename(char *s)
{
    char base[256];
    int i =0;
    char *path = s;
    int j = strlen(s);

    //printf("base main: %s\n", s);

    while (*path)
    {
        //printf("%s\n", path);
        path++;
    }

    while (*path != '/' && path != s)
    {
        path--;
    }

    if (*path == '/')
    {
        path++;
    }

    return path;
/*
    while(*path)
    {
        printf("%c", *path);
        base[i] = *path;
        i++;
        path++;
    }

    printf("\ni = %d\n", i);

    //base[i+2] = '\0';

    return &base;
    */
}


int main(int argc, char* argv[])
{
    STAT sr;
    STAT de;
    STAT pa;

    char *src = argv[1];
    char *dest = argv[2];

    char *path;
    char *base;

    char *cmd;

    int sfd;
    int dfd;
    int dpfd;

    char *tmp;// = malloc(strlen(dest)+strlen(base)+1);

    base = basename(src);
    path = dirname(dest);

    //printf("src: %s, dest: %s\n", src, dest);

    cmd = malloc(strlen("rm ")+strlen(dest)+1);
    tmp = malloc(strlen(dest)+strlen(base)+1);

    sfd = stat(src, &sr);
    dfd = stat(dest, &de);
    dpfd = stat(path, &pa);

    //printf("path: %s, base: %s\n", path, base);

	if(argc == 3)
	{
        //make sure that source exists and dirname(dest) exists  

        //printf("path : %d\ndpfd : %d\nsfd : %d\nresult: %d\n", strlen(path), dpfd, sfd, sfd > 0 && (dpfd > 0 | !strlen(path)));
        if(sfd >= 0 && dpfd >= 0) //for instance, dest = "(./)file_under_the_curre"
        {

            if(S_ISDIR(sr.st_mode))
            {
                //cp from src to dest
                if(dfd >= 0) //dest exists
                {
                    if(S_ISDIR(de.st_mode)) //if it is dir also, cd it. else, delete it 
                    {
                        //cd
                        strcpy(tmp, dest);
                        strcat(tmp, "/");
                        strcat(tmp, base);
                        mkdir(tmp);
                        copy_them(src, tmp);
                    }
                    else //it is a file
                    {
                        /*
                        //delete
                        strcpy(cmd, "rm ");
                        strcat(cmd, dest);
                        exec(cmd);
                        mkdir(dest);
                        printf("src is dir, dest is dir\n");

                        //copy it
                        copy_them(src, dest);
                        */

                        printf("error! cannot cp from dir to file!!!\n");

                    }
                }
                else
                {
                    //cp from src and new basename(dest)

                    mkdir(dest);
                    copy_them(src, dest);                    
                    
                }
            }
            else
            {
                if(dfd >= 0) //dest exists
                {
                    //doesn't matter if src is just a file
                    if(S_ISDIR(de.st_mode)) //if it is dir also, cd it. else, delete it 
                    {
                        //cd
                        strcpy(tmp, dest);
                        strcat(tmp, "/");
                        strcat(tmp, base);
                        copy_it(src, tmp);
                        exit(0);
                    }
                    else
                    {                
                        //printf("file to file\n");
                        copy_it(src, dest);
                    }
                }

                else
                {
                    //printf("but parent dir still exists\n");
                    copy_it(src, dest);
                }
            }
        }
        else
        {
            printf("The '%s' or '%s' do not exist!\n", src, path);
        }
	}
	else
	{
		printf("error\n");
	}

    return 0;
}

int myread(int infile, int outfile)
{
    int byteRead = -1;
    int total = 0;

    char *buff[BLKSIZE];

    while(byteRead != 0)
    {
        //printf("success from copy it\n");
        byteRead = read(infile, buff, BLKSIZE);
        write(outfile, buff, byteRead);
        total += byteRead;
    }

    return total;
}

copy_it(char *src, char *dest)
{
     int infile = 0;
     int outfile = 1;

     char mybuff[BLKSIZE];
     char *tmp;

     STAT sr;
     int sfd = stat(src, &sr);

     int i = 0;

     //printf("stat (%s) is %d\n", src, sfd);

     if(filesEqual(src, dest))
     {
        printf("cannot copy from %s to %s: same file!!!\n", src, dest);
        return 0;
     }

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

     outfile = open(dest, 1 | 64);

     //printf("the dest is %s\n", dest);

     i = myread(infile, outfile);

     /*
     printf("total read: %d\n", i);
     printf("infile : %d\n", infile);
     printf("outfile: %d\n", outfile);
     */

     close(infile);
     //printf("closed infile\n");
     close(outfile);
     //printf("closed outfile\n");
}

int filesEqual(char *f1, char *f2)
{
    STAT st1, st2;
    int s1, s2;

    s1 = stat(f1, &st1);
    s2 = stat(f2, &st2);
    if (s1 < 0)
    {
        return 0;
    }

    if (s2 < 0)
    {
        return 0;
    }

    if ((st1.st_dev == st2.st_dev) && (st1.st_ino == st2.st_ino))
    {
       return 1;
    }

    return 0;
}

copy_them(char *src, char *dest)
{
    STAT st;

    int infile = 0;
    int outfile = 1;

    int byteRead = -1;

    char buf[BLKSIZE];
    char name[BLKSIZE];

    char *cp;
    DIR *dp;

    char *sr[BLKSIZE];
    char *de[BLKSIZE];

    int i;

    if(filesEqual(src, dest))
    {
       printf("cannot copy from %s to %s: same file!!!\n", src, dest);
       return 0;
    }

    infile = open(src, 0);

    while(byteRead = read(infile, buf, BLKSIZE))
    { 
        dp = (DIR *)buf;
        cp = buf;
        //printf("success from copy_them\n");
        while (cp < &buf[BLKSIZE]) 
        {
            strncpy(name, dp->name, dp->name_len);
            name[dp->name_len] = '\0';

            //skip the trouble makers
            if (strcmp(name, ".") == 0 || strcmp(name, "..") == 0) {
                cp += dp->rec_len;
                dp = (DIR *)cp;
                continue;
            }

            strcpy(sr, src); 
            strcat(sr, "/");
            strcat(sr, name);
            
            strcpy(de, dest); 
            strcat(de, "/"); 
            strcat(de, name);

            i = stat(sr, &st);

            if(i>=0)
            {
                //printf("found : %s\n", sr);
                if (S_ISREG(st.st_mode) || S_ISLNK(st.st_mode)) {
                    copy_it(sr, de);
                }

                if (S_ISDIR(st.st_mode)) {
                    mkdir(de);
                    copy_them(sr, de);
                }
            }

            else
            {
                printf("not found : %s\n", sr);
            }


            cp += dp->rec_len;
            dp = (DIR *)cp;
        }
    
    }

    close(infile);
    exit(0);
}