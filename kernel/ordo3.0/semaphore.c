#include "semaphore.h"


typedef struct sem
{
	int counter;
	ctx* waiting_threads;
}sem;
static sem* semaphores[256] = {0};

int new_semaphore(int init)
{
	int id;
	sem *new;
	disable_it();
	new = malloc(sizeof(sem));
	new->counter = init;
	new->waiting_threads = NULL;
	for (id = 0; semaphores[id] != 0; id++);
	semaphores[id] = new;
	enable_it();
	return id;
}

int del_semaphore(int id)
{
	disable_it();
	if (semaphores[id]->waiting_threads != NULL)
	{
		printf("Le semaphore numero %d ne peut pas etre detruit: il n'a pas tous ces jetons\n",id);
		irq_enable();
		return -1;
	}
	else
	{
		free(semaphores[id]);
		semaphores[id] = 0;
	}
	enable_it();
	return 1;
}


void sem_take(int sem_id)
{
	sem *sema;
	ctx *save;
	disable_it();
	if (((unsigned)sem_id > 255) || semaphores[sem_id] == 0)
	{
		printf("The semaphore number %d does not exist\n",sem_id);
	}
	else
	{
		sema = semaphores[sem_id];
		if (sema->counter > 0)
		{
			sema->counter -= 1;
		}
		else
		{
			save = current_thread;
			disable_current_thread();
			save->next = sema->waiting_threads;
			sema->waiting_threads = save;
			scheduler(save);
		}
	}
	enable_it();
}

void sem_give(int sem_id)
{
	sem *sema;
	ctx *save;
	disable_it();
	if (((unsigned)sem_id > 255) || semaphores[sem_id] == 0)
	{
		printf("The semaphore number %d does not exist\n",sem_id);
	}
	else
	{
		sema = semaphores[sem_id];
		if(sema->waiting_threads == NULL)
		{
			sema->counter += 1;
		}
		else
		{
			save = sema->waiting_threads;
			sema->waiting_threads = save->next;

			save->next = current_thread->next;
			save->last = current_thread->last;

			current_thread->next = save;
			save->next->last = save;
		}
	}
	enable_it();
}
