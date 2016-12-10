#include "../headers/type.h"

char *ctable = "0123456789ABCDEF";
u16 BASE = 10;


int geti()
{
  int i;
  char *c = 0;
  
  c = gets();
  i = atoi(c);
  
  return i;
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

/*
int rpu(u16 x)
{
	char c;
	if (x) {
		c = ctable[x % BASE];
		rpu(x / BASE);
		putc(c);
	}
}

int rpul(u32 x)
{
	char c;
	if (x) {
		c = ctable[(u32)x % BASE];
		rpu((u32)x / BASE);
		putc(c);
	}
}

int printu(u16 x)
{
	BASE = 10;
	if (x==0)
		putc('0');
	else
		rpu(x);
	putc(' ');
}

int printx(u16 x)
{
	BASE = 16;
	if (x==0)
		putc('0');
	else
		rpu(x);
	putc(' ');
}

int printl(u32 x)
{
	BASE = 10;
	if (x==0)
		putc('0');
	else
		rpul((u32)x);
	putc(' ');
}

int prints(char *s)
{
	while(*s)
		putc(*s++);
}

int printf(char *fmt, ...)
{
	char *cp = fmt;
	u16 *ip = (int *)&fmt +1;
	u32 *up;

	while(*cp!='\r')
	{
		if (*cp=='%') {
			switch(++*cp){
				case 'c':
					putc(*ip);
					ip+=2;
					break;
				case 's':
					prints(*ip);
					ip+=2;
					break;
				case 'd':
					printu(*ip);
					ip+=2;
					break;
				case 'l':
					printl(*ip);
					ip+=4;
					break;
				default:
					putc(cp);
					break;
			}
		}
		else if (*cp++=='\n')
			putc('\r');
		else{
			putc(*cp++);
		}
	}
}
// ksleep(), kwakeup(), kexit(), wait()
*/
