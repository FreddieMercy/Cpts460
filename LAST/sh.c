/*==========================================================
                   sh Program:
YOUR sh must support
(1). logout/exit :  terminate
(2). simple command exceution, e.g. ls, cat, ....
(3). I/O redirection:  e.g. cat < infile;  cat > outfile, etc.
(4). (MULTIPLE) PIPEs: e.g. cat file | l2u | grep LINE 
===========================================================
*/

#include "ucode.c"

main(int argc, char *argv[])   // invoked by exec("login /dev/ttyxx")
{
	char cmd[256];
	char cmd1[256];
	char path[256];
	int i = 0;
	int j = 0;
	int z = 0;
	int m = 0;
	char *cd = "cd";
	char *logout = "logout";
	char *su = "su";

	while(1)
	{
		printf("Freddie wants you to input cmd: ");
		gets(cmd);
		//printf("gets: %s\n", cmd);

		//get cmd token from command line

		for(i = 0; i < strlen(cmd); i++)
		{
			if(cmd[i] == ' ')
			{
				z = 1;
			}
			else if(!z)
			{
				cmd1[j] = cmd[i];
				j++;
			}
			else
			{
				path[m] = cmd[i];
				m++;
			}
		}

		cmd1[j] = '\0';
		i = 0;
		j = 0;
		path[m] = '\0';
		m = 0;
		z = 0;

		if(strcmp(cmd1, cd) == 0) 
		{
			chdir(path);
			//printf("exec\n");
			continue;
		}

		if(strcmp(cmd1, su) == 0)
		{
			//exec(cmd);
			break;
		}

		if(strcmp(cmd1, logout) == 0)
		{
			//printf("break\n");
			break;
		}

		pid = fork();

	    if (pid < 0)
        {
            printf("fork failed!\n");
            
            return -1;
        }

		if(pid)
		{
			if(!findAnd(&cmd))
			{
				pid = wait(&status);
				//exit(0);
			}
			continue;
		}
		else
		{
			//printf("main do_pipe\n");
			do_pipe(&cmd, 0);

		}
	}

}

int findAnd(char *cmdLine)
{
	int i = 0;

	for(i = 0; i < strlen(cmdLine); i++)
	{
		if(cmdLine[i] == '&')
		{
			return 1;
		}
	}

	return 0;
}

int do_pipe(char *cmdLine, int *pd)
{
	int hasPipe;
	char *head[256];
	char *tail[256];
	int lpd[2];

	if(pd)
	{
		close(pd[0]);
		dup2(pd[1], 1);
		close(pd[1]);
	}

	//printf("doing hasPipe\n");
	hasPipe = scan(cmdLine, head, tail);
	//printf("Finish doing hasPipe, hasPipe = %d\n", hasPipe);
	if(hasPipe)
	{
		//create a pipe lpd;
		pipe(lpd);

		pid = fork();
		if(pid)
		{
			//as reader on lpd:
			close(lpd[1]);
			dup2(lpd[0], 0);
			close(lpd[0]);

			//printf("tail len: %d, tail is : %s\n tail isAllSpace = %d\n", strlen(tail), tail, isAllSpace(tail));
			
			if(strlen(tail) > 0 && !isAllSpace(tail))
			{
				//printf("doing command: %s\n", tail);
				do_command(tail);	
			}	
		}
		else
		{
			//printf("do_pipe do_pipe\n");
			//printf("head len: %d, head is : %s\n head isAllSpace = %d\n", strlen(head), head, isAllSpace(head));
			if(strlen(head)>0 && !isAllSpace(head))
			{
				//printf("forking pipe: %s\n", head);
				do_pipe(head, lpd);
			}
			//exit(0);
		}
	}
	else
	{
		do_command(cmdLine);
	}
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

	int fd;

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
		else if(check > 0 && cmdLine[i] != ' ')
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

		fileName[j]='\0';
		cmdName[z]='\0';

	}

	//printf("i = %d, cmdLine: %s, cmd: %s, file = %s\n",i ,cmdLine, cmdName, fileName);
	
	if (check > 0)
	{
			//close file??
			if (check == 1)
			{
				//printf("found '<'\n");
				close(0);
				fd = open(fileName, O_RDONLY);
				//dup2(fd, 0);
				//close(fd);
			}
			else if (check == 2)
			{
				//printf("found '>'\n");
				close(1);
				fd = open(fileName, O_WRONLY | O_CREAT);
				//dup2(fd, 1);
				//close(fd);
			}
			
			else if (check == 3)
			{
				//printf("found '>>'\n");
				close(1);
				fd = open(fileName, O_APPEND | O_WRONLY | O_CREAT);
				//dup2(fd, 1);
				//close(fd);
			}
	}
	
	// otherwise we finally exec()
	exec(cmdName);
	
	// /exit(0);
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
/*
		printf("(return 0)new head: ");

		printf("%s", head);

		printf(", tail: %s\n", tail);
*/
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

		//printf("(return 1)head: %s, tail: %s\n", head, tail);

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

int isAllSpace(char *input)
{
	int i = 0;
	//printf("\nis all space:");
	for(i = 0; i < strlen(input); i++)
	{
		//printf("%c", input[i]);
		if(input[i] != ' ')
		{
			return 0;
		}
	}

	return 1;
}