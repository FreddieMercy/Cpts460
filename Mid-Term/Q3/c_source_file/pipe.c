/********************** Unix Pipe Example *************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../headers/type.h"
#include "../headers/int.h"
#include "../headers/wait.h"
#include "../headers/pipe.h"

int kpipe()
{
	int i;
	PIPE *pipe;
	OFT *ofts0;
	OFT *ofts1;

	pipe = 0;

	for(i = 0; i < NPIPE; i++)
	{
		if(pipes[i].status == FREE)
		{
			pipe = &pipes[i];
			pipe->status = READY;
			pipe->head = 0;
			pipe->tail = 0;
			pipe->data = 0;
			pipe->room = PSIZE;
			pipe->nreader = 1;
			pipe->nwriter = 1;

			break;
		}
	}

	if(!pipe)
	{
		printf("No available pipe. failed!!\n");
		return -1;
	}

	ofts0 = 0;
	
	for(i = 0; i < NFD; i++)
	{
		if(ofts[i].status == FREE)
		{
			ofts0 = &ofts[i];
			ofts0->mode = READ_PIPE;
			//ofts1.mode = WRITE_PIPE;

			ofts0->refCount = 1;
			//ofts1.refCount = 1;

			ofts0->pipe_ptr = pipe;
			ofts0->status = READY;
			//ofts1.pipe_ptr = &pipe;

			break;
		}
	}

	if(!ofts0)
	{
		printf("No available ofts. failed!!\n");
		return -1;
	}

	ofts1 = 0;
	
	for(i = 0; i < NFD; i++)
	{
		if(ofts[i].status == FREE)
		{
			ofts1 = &ofts[i];
			//ofts0->mode = READ_PIPE;
			ofts1->mode = WRITE_PIPE;

			//ofts0->refCount = 1;
			ofts1->refCount = 1;

			//ofts0->pipe_ptr = pipe;
			ofts1->status = READY;
			ofts1->pipe_ptr = pipe;

			break;
		}
	}

	if(!ofts1)
	{
		printf("No available ofts. failed!!\n");
		return -1;
	}

	for(i = 0; i < NFD; i+=2)
	{
	    if(running->fd[i]->status == FREE && running->fd[i + 1]->status == FREE)
	    {	    	
			//pd[0] = i;
			running->fd[i] = ofts0;
		
			//pd[1] = i+1;
			running->fd[i+1] = ofts1;
	    	
	    	printf("Read fd = %d, Write fd = %d\n", i, i+1);

	    	return 0;
	    }

	    printf("%d, %d\n", running->fd[i]->status, running->fd[i + 1]->status);
	    
	}

	printf("No available fd, failed!\n");
	//no available fds
	return -1;
}

int close_pipe(int fd)
{
	OFT *fdptr;
	PIPE *pipe_ptr;
	int data;

	if(!running->fd[fd])
	{
		printf("fd = %d is not a valid opened file descriptor\n", fd);
		return -1;
	}

	fdptr = running->fd[fd];
	fdptr->refCount--;

	if(fdptr->mode == WRITE_PIPE)
	{
		data = pipe_ptr->data;
		pipe_ptr = fdptr->pipe_ptr;
		pipe_ptr->nwriter--;
		if(!pipe_ptr->nwriter && !pipe_ptr->nreader)
		{
			free(pipe_ptr);
			fdptr->pipe_ptr = 0;
			free(fdptr);
			running->fd[fd] = 0;
		}

		kwakeup(data);
		// wakeup ALL blocked readers
	}
	else
	{
		data = pipe_ptr->data;
		pipe_ptr = fdptr->pipe_ptr;
		pipe_ptr->nreader--;
		if(!pipe_ptr->nwriter && !pipe_ptr->nreader)
		{
			free(pipe_ptr);
			fdptr->pipe_ptr = 0;
			free(fdptr);
			running->fd[fd] = 0;
		}

		kwakeup(data);
	}

	running->fd[fd] = 0;

	printf("pipe fd = %d closed\n", fd);

	return 0;

}

int read_pipe(int fd, char *buf, int n)
{
	int r = 0;
	char c = '\0';
	OFT *fdptr;
	PIPE *pipe_ptr;

	if(n<=0)
	{
		printf("n <= 0\n");
		return 0;
	}

	if(!running->fd[fd])
	{
		printf("fd = %d is not a valid opened file descriptor\n", fd);
		return -1;
	}

	fdptr = running->fd[fd];
	pipe_ptr = fdptr->pipe_ptr;

	printf("before read:\nnwriter:%d     nreader:%d     refCount:%d     data:%d     room:%d     \ncontent: %ss\n",pipe_ptr->nwriter, pipe_ptr->nreader, fdptr->refCount, pipe_ptr->data, pipe_ptr->room, pipe_ptr->buf);

	while(n)
	{
		while(pipe_ptr->data)
		{
			c = pipe_ptr->buf[pipe_ptr->tail];
			put_byte(c, running->uss, buf);

			n--;
			r++;
			pipe_ptr->data--;
			pipe_ptr->room++;
			if(!n)
			{
				break;
			}
		}

		if(r)
		{
			kwakeup(pipe_ptr->room);

			printf("after read:\nnwriter:%d     nreader:%d     refCount:%d     data:%d     room:%d     \ncontent: %ss\n",pipe_ptr->nwriter, pipe_ptr->nreader, fdptr->refCount, pipe_ptr->data, pipe_ptr->room, pipe_ptr->buf);

			return r;
		}

		if(pipe_ptr->nwriter)
		{
			kwakeup(pipe_ptr->room);
			ksleep(pipe_ptr->data);
			continue;
		}
		else
		{
			printf("after read:\nnwriter:%d     nreader:%d     refCount:%d     data:%d     room:%d     \ncontent: %ss\n",pipe_ptr->nwriter, pipe_ptr->nreader, fdptr->refCount, pipe_ptr->data, pipe_ptr->room, pipe_ptr->buf);
			return 0;
		}
	}

	printf("after read:\nnwriter:%d     nreader:%d     refCount:%d     data:%d     room:%d     \ncontent: %ss\n",pipe_ptr->nwriter, pipe_ptr->nreader, fdptr->refCount, pipe_ptr->data, pipe_ptr->room, pipe_ptr->buf);

}

int write_pipe(int fd, char *buf, int n)
{
	int r = 0;
	OFT *fdptr;
	PIPE *pipe_ptr;

	if(n<=0)
	{
		printf("n <= 0\n");
		return 0;
	}

	if(!running->fd[fd])
	{
		printf("fd = %d is not a valid opened file descriptor\n", fd);
		return -1;
	}

	fdptr = running->fd[fd];
	pipe_ptr = fdptr->pipe_ptr;
	printf("before write:\nnwriter:%d     nreader:%d     refCount:%d     data:%d     room:%d     \ncontent: %ss\n",pipe_ptr->nwriter, pipe_ptr->nreader, fdptr->refCount, pipe_ptr->data, pipe_ptr->room, pipe_ptr->buf);

	while(n)
	{
		if(!pipe_ptr->nreader)
		{
			printf("Broken Pipe!!\n");
			kexit(BROKEN_PIPE);
		}

		while(pipe_ptr->room)
		{

			pipe_ptr->buf[pipe_ptr->head] = get_byte(running->uss, buf);

			n--;
			r++;
			pipe_ptr->data++;
			pipe_ptr->room--;
			if(!n)
			{
				break;
			}
		}

		kwakeup(pipe_ptr->data);
		if(!n)
		{
			printf("after write:\nnwriter:%d     nreader:%d     refCount:%d     data:%d     room:%d     \ncontent: %ss\n",pipe_ptr->nwriter, pipe_ptr->nreader, fdptr->refCount, pipe_ptr->data, pipe_ptr->room, pipe_ptr->buf);

			return r;
		}

		ksleep(pipe_ptr->room);
	}
	printf("after write:\nnwriter:%d     nreader:%d     refCount:%d     data:%d     room:%d     \ncontent: %ss\n",pipe_ptr->nwriter, pipe_ptr->nreader, fdptr->refCount, pipe_ptr->data, pipe_ptr->room, pipe_ptr->buf);

}

/*
int pipe_demo()
{
	kpipe(pd);
	// create a pipe
	if (fork()){
		// fork a child as READER, parent as WRITER
		printf("parent %d close pd[0]\n", kgetpid());
		close(pd[0]);
		while(i++ < 10){ 

			// parent writes to pipe t0 times
			printf("parent %d writing pipe : %s\n", kgetpid(), s);
			write(pd[1], s, strlen(s));
		}
		printf("parent %d exit\n", kgetpid());
		kexit(0);
	}
	else{
		// child as pipe READER
		printf("child %d close pd[1]\n", kgetpid());
		close_pipe(pd[1]);
		while(1){
			// child read from pipe
			printf("child %d reading from pipe\n", kgetpid());
			if ((n = read(pd[0], line, 256)) == 0)
			kexit(0);
				line[n]=0; printf("%s n=%d\n",line, n);
		}
	}
}
*/

int pfd()
{
	int i;
	OFT *fdptr;
	//PIPE *pipe_ptr;

	for(i = 0; i < NFD; i++)
	{
		fdptr = running->fd[i];
		if(fdptr!=0)
		{

			if(fdptr->mode == WRITE_PIPE)
			{
				printf("WRITE_PIPE   refCount=%d\n", fdptr->refCount);
			}
			else
			{
				printf("READ_PIPE    refCount=%d\n", fdptr->refCount);
			}
		}
	}
}