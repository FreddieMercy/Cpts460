
#include <stdio.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

int main(int argc, char* argv[])
{
	int fd;
	int i = 0;
	char buff1[256];
	char buff2[256];
	char *head = &buff1;
	char *tail = &buff2;

	tail[0] = 'F';
	tail[1] = 'r';
	tail[2] = 'e';
	tail[3] = 'd';
	tail[4] = '\0';

	for (i=0; i<argc; i++){
	    printf("argv[%d]=%s\n", i, argv[i]);
	  }

	if(argc > 1)
	{
		printf("parsing %s...\n", argv[1]);

		//printf("tail: %d\n", tail);
		parseCMD(argv[1], &head, &tail);	

	 	printf("head: %s\n", head);
		printf("tail: %s\n", tail);
			
		
		i = 0;
		while(tail != 0)
		{
			//printf("tail: %d\n", tail);
			
			parseCMD(head, &head, &tail);
			printf("head: %s\n", head);
			printf("tail: %s\n", tail);
			i++;
		}
		
			
	}
	else
	{
		printf("error\n");
	}

	return 0;
}

parseCMD(char *cmdLine, char **heads, char **tails)
{
	int i;
	int j = 0;
	char *head = *heads;
	char *tail = *tails;

	//printf("the tail is %s\n", tail);

	for(i = strlen(cmdLine) - 1; i>=0; i--)
	{

		if(cmdLine[i] != '|')
		{
			tail[j] = cmdLine[i];
			j++;

			//printf("%c, %s, %d\n", cmdLine[i], tail, j);
		}

		else
		{
			//printf("success\n");
			break;
		}
		
	}


	tail[j] = '\0';

	flipTail(tail);

	//printf("i = %d\n", i);

	if(i <= 0) //when i = 0, for the cmd: "| cmd paramtr" (pipe symbol is the first char in the cmdLine)
	{
		for(i = 0; i < strlen(tail); i++)
		{
			head[i] = tail[i];
		}

		//printf("***************************************************\n");
		//tail[0] = '\0';
		tail = 0;

		//printf("after null: %s\n", tail);
	}
	else
	{
		head[i] = '\0';
		i--;
		while(i>=0)
		{
			head[i] = cmdLine[i];
			i--;
		}
	}


	*heads = head;

	*tails = tail;
}

flipTail(char *tail)
{
	char tmp[256];
	int i = strlen(tail) - 1;
	int j = 0;
	//printf("from flip, tail = %s, length = %d\n", tail, strlen(tail));

	for(j = 0; j < strlen(tail); j++)
	{
		tmp[j] = tail[i];
		//printf("tmp: %c\n", tmp[j]);
		i--;
	}

	tmp[j] = '\0';

	for(j = 0; j < strlen(tmp); j++)
	{
		tail[j] = tmp[j];
	}

	//printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!%s\n", tmp);
}