
#include <stdlib.h>

typedef unsigned int i32;
int BASE = 10;

char *table = "0123456789ABCDEF";

int rpu(i32 x, int base)
{
  char c;
  if (x){
     c = table[x % base];
     rpu(x / base, base);
     putc(c);
  }
}

int printu(i32 x, int base)
{
  if (x==0)
     putc('0');
  else
     rpu(x, base);
  putc(' ');
}

int prints(char *s)
{
    int i;
    for(i = 0; s[i] != '\0'; i++)
    {
        putc(s[i]);
    }
}

int printd(int x)//: print an integer (which may be negative!!)
{
    if(x == 0)
    {
        putc('0');
    }
    else if(x > 0)
    {
        printu(x, 10);
    }
    else
    {
        putc('-');
        printu(~x +1, 10);
    }
}

int printo(i32 x)//: print x in OCTal as 0.....
{
    putc('0');
    if(x == 0)
    {
        putc('0');
    }
    else if(x > 0)
    {
        printu(x, 8);
    }
    else
    {
        putc('-');
        printu(~x +1, 8);
    }

}

int printx(i32 x)//: print x in HEX.  as 0x....
{
    putc('0');
    putc('x');
    if(x == 0)
    {
        putc('0');
    }
    else if(x > 0)
    {
        printu(x, 16);
    }
    else
    {
        putc('-');
        printu(~x +1, 16);
    }
}

int printl(i32 x)//: print x in HEX.  as 0x....
{
    if(x == 0)
    {
        putc('0');
    }
    else if(x > 0)
    {
        printu(x, 32);
    }
    else
    {
        putc('-');
        printu(~x +1, 32);
    }
}

int printX(i32 x)//: print x in HEX.  as 0x....
{
    putc('0');
    putc('x');
    if(x == 0)
    {
        putc('0');
    }
    else if(x > 0)
    {
        printu(x, 32);
    }
    else
    {
        putc('-');
        printu(~x +1, 32);
    }
}


int printf(char *fmt, ...) // SOME C compiler requires the 3 DOTs
{
    char *cp = fmt;
    char temp;
    int *ip = &fmt + 1;
    int i;
    for(i = 0; fmt[i] != '\0'; i++)
    {
        if(fmt[i] == '%')
        {
            i++;
            temp = fmt[i];

            switch(temp)
            {
                case 'c':
                    putc(*ip);
                    break;
                case 's':
                    prints(*ip);
                    break;
                case 'u':
                    printu(*ip, 10);
                    break;
                case 'd':
                    printd(*ip);
                    break;
                case 'o':
                    printo(*ip);
                    break;
                case 'x':
                    printx(*ip);
                    break;
                case 'l':
                    printl(*(i32 *)ip++);
                    break;
                case 'X':
                    printX(*(i32 *)ip++);
                    break;
                default:
                    putc(*ip);
                    break;
            }
            ip++;
        }
        else
        {
            putc(fmt[i]);
        }
    }
}

int gets(char *in)
{
    int i = 0;
    char tmp = '\0';

    while(tmp !='1')
    {

        tmp = getc();
        if(i<64 & tmp !='1'){
            printf("%c",tmp);
            in[i]=tmp;
            i++;
        }else if (i>=64){
            printf("Max char 64\n");
        }else{
            printf("Entered newline\n");
            break;
        }
    }

    if(i < 64)
    {
        in[i]='\0';
    }

    printf("\nTest string : %s\n", in);
}

