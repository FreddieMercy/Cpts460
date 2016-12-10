
#include <stdio.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

int main(int argc, char* argv[])
{

	if(argc > 1)
	{
		do_command(argv[1]);
		
			
	}
	else
	{
		printf("error\n");
	}

	return 0;
}

int do_command(char *cmdLine)
{
	char fileName[256];
	char cmdName[256];
	int rep = 0;
	int i = 0;
	int j = 0;
	int z = 0;
	int check = -1;

	for(i = 0; i < strlen(cmdLine); i++)
	{
		//assuming that here is only ONE redir sign
		if(cmdLine[i] == '>' && cmdLine[i+1] == '>')
		{
			i++;
			check = 3;
		}
		else if(cmdLine[i] == '<')
		{
			check = 1;
		}
		else if(cmdLine[i] == '>')
		{
			check = 2;
		}
		else if(check > 0)
		{
			fileName[j] = cmdLine[i];
			//printf("1. %s\n", fileName);
			j++;
		}
		else
		{
			cmdName[z] = cmdLine[i];
			//printf("z = %d, %c\n",z, cmdName[z]);
			//printf("i = %d, %c\n",i, cmdLine[i]);
			z++;
		}

	}

	printf("i = %d, cmdLine: %s, cmd: %s, file = %s\n",i ,cmdLine, cmdName, fileName);
	
	if (check > 0)
	{
			if (check == 1)
			{
				printf("found '<'\n");
				//close(0);
				//open(fileName, O_RDONLY);
			}
			else if (check == 2)
			{
				printf("found '>'\n");
				//close(1);
				//open(fileName, O_WRONLY | O_CREAT);
			}
			
			else if (check == 3)
			{
				printf("found '>>'\n");
				//close(1);
				//open(fileName, O_APPEND | O_WRONLY | O_CREAT);
			}
	}
	
	// otherwise we finally exec()
	//exec(cmdName);
	
	return 0;
}

int scan(char *cmdLine, char *head, char *tail)
{
	/*
		divide cmdLine into head and tail by rightmost | symbol
		cmdLine = cmd 1 | cmd 2 | ... cmdn-1 | cmdn
			return 1;
		cmdLine = cmd1 => head = cmdl, tail  = null; 
			return 0;
	*/


	int i;
	int j = 0;

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
		tail[0] = '\0';
		//tail = 0;

		//printf("after null: %s\n", tail);

		return 0;
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

		return 1;
	}
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


}