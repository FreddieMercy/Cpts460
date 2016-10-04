#include "../headers/type.h"

//enter p into list
int put_proc(PROC **lists, PROC *p)
{
	PROC *list = lists;
	PROC *tmp = list;

	while(list->next != 0)
	{
		list=list->next;
	}

	list->next=p;

	list = tmp;
}

//return a FREE PROC pointer from list
PROC *get_proc(PROC **list)
{
	PROC *tmp = *list;

	if(!tmp)
	{
		return 0;

	}

	else
	{
		
		*list = tmp->next;
		tmp->next = 0;
		return tmp;

	}
	
}

//enter p into queue by priority
int enqueue(PROC **queue, PROC *p)
{
	PROC *tmp = *queue;
	PROC *origin = tmp;


	p->next = 0;

	if(!tmp)
	{
		p->next = 0;
		*queue = p;

	}
	else
	{
		PROC *pre = 0;
		while(tmp->priority <= p->priority && tmp->next !=0)
		{
			pre = tmp;
			tmp = tmp->next;
		}

		if(tmp->priority > p->priority)
		{
			if(!pre)
			{
				pre->next = p;
				p->next = tmp;
			}
			else
			{
				//if queue is the first one and queue->priority > p->priority
				p->next = tmp;
				tmp=p;
			}

		}
		else
		{
			p->next = tmp->next;
			tmp->next=p;
		}
		
		*queue = origin;

	}
}

//return first element removed from queue
PROC *dequeue(PROC **queues)
{
	PROC *queue = *queues;
	PROC *tmp = queue;
	*queues = queue->next;

	if(!tmp)
	{
		return 0;
	}
	else
	{
		tmp->next = 0;
		return tmp;		
	}
}

//print name=list contents
printList(char *name, PROC *list)
{
	PROC *tmp = list;
	
	printf("%s: ", name);
	
	if(!list)
	{
		printf("NULL\n");

	}
	else
	{
		while(list->next!=0)
		{
			printf("%d -> ", list->pid);
			//printf("%d (priority = %d) -> ", list->pid, list->priority);
			list = list->next;
		}

		printf("%d -> NULL\n", list->pid);
		//printf("%d (priority = %d) -> ", list->pid, list->priority);

		list = tmp;
	}
}

printQ(char *name, PROC *list)
{
	PROC *tmp = list;
	
	printf("%s: ", name);
	
	if(!list)
	{
		printf("NULL\n");

	}
	else
	{
		while(list->next!=0)
		{
			printf("%d -> ", list->pid);
			//printf("%d (priority = %d) -> ", list->pid, list->priority);
			list = list->next;
		}

		printf("%d -> NULL\n", list->pid);
		//printf("%d (priority = %d) -> ", list->pid, list->priority);

		list = tmp;
	}
}