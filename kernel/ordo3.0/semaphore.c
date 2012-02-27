#include "semaphore.h"


/*
 * semaphore structure
 */
typedef struct sem
{
	// number of coins
	int counter;
	// lined list of thread in waiting
	ctx* waiting_threads;
}sem;

// table of semaphores
static sem* semaphores[256] = {0};

/*
 * Create a new semaphore
 * parameter:
 * 	init: initial number of coins
 * return value: semaphore id
 */
int new_semaphore(int init)
{
	int id;
	sem *new;
	// enter in safe mode
	disable_it();
	// allocating a new semaphore
	new = malloc(sizeof(sem));
	new->counter = init;
	new->waiting_threads = NULL;
	// look for a free id
	for (id = 0; semaphores[id] != 0; id++);
	semaphores[id] = new;
	//leave safe mode
	enable_it();
	return id;
}

/*
 * Delete a semaphore
 * parameter:
 * 	id: semaphore id
 * return value: -1 if an error occured, 1 otherwise
 */
int del_semaphore(int id)
{
	// enter in safe mode
	disable_it();
	// check if a thread is waiting
	if (semaphores[id]->waiting_threads != NULL)
	{
		printf("Le semaphore numero %d ne peut pas etre detruit: il y a thread bloqué decus \n",id);
		// leave safe mode
		irq_enable();
		// return error
		return -1;
	}
	else
	{
		// delete the semaphore
		free(semaphores[id]);
		semaphores[id] = 0;
	}
	// leave safe mode 
	enable_it();
	// return succes
	return 1;
}


/*
 * Take a coin from a semaphore
 * This function block until a coin is available
 * parameter:
 * 	sem_id: semaphore id
 */
void sem_take(int sem_id)
{
	sem *sema;
	ctx *save;
	// enter in safe mode
	disable_it();
	// check if the sem_id is ok
	if (((unsigned)sem_id > 255) || semaphores[sem_id] == 0)
	{
		printf("The semaphore number %d does not exist\n",sem_id);
	}
	else
	{
		sema = semaphores[sem_id];
		// if the semaphore have a coin
		if (sema->counter > 0)
		{
			// it simply decrease the number of coin
			sema->counter -= 1;
		}
		else
		{
			save = current_thread;
			// suspend the current thread
			disable_current_thread();
			// chain the thread in the semaphore linked list
			save->next = sema->waiting_threads;
			sema->waiting_threads = save;
			// call the scheduler
			scheduler(save);
		}
	}
	// leave safe mode
	enable_it();
}

/*
 * Give a coin to a semaphore
 * parameter:
 * 	sem_id: semaphore id
 */
void sem_give(int sem_id)
{
	sem *sema;
	ctx *save;
	// enter in safe mode
	disable_it();
	// check if the sem_id is ok
	if (((unsigned)sem_id > 255) || semaphores[sem_id] == 0)
	{
		printf("The semaphore number %d does not exist\n",sem_id);
	}
	else
	{
		sema = semaphores[sem_id];
		// if there are no thread in waiting
		if(sema->waiting_threads == NULL)
		{
			// increase the number of coin
			sema->counter += 1;
		}
		else
		{
			// unlink the first waiting thread
			save = sema->waiting_threads;
			sema->waiting_threads = save->next;

			// add this thread to activable thread
			save->next = current_thread->next;
			save->last = current_thread->last;
			current_thread->next = save;
			save->next->last = save;
		}
	}
	// leave safe mode
	enable_it();
}
