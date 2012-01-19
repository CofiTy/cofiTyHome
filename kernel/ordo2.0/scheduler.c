#include "scheduler.h"

/* Time for RoundRobin, sec in int and milli in int */
#define SWITCH_LAPSE_SEC 1
#define SWITCH_LAPSE_MILLI 0

typedef struct gThread
{
	struct gThread *next;
	mctx_t context;
	int id;
	int timeToWait;
	char *stack;
} gThread;

static gThread *firstThread = NULL;
static gThread *currentThread = NULL;
static gThread *threadForDeletion = NULL;
static gThread *threadInWaitingState = NULL;
/*static gThread *sleepingThread = NULL;*/
static int counter = 0;
volatile int itEnabled = FALSE;
static int removeGThreadFromActivable(gThread* toRemove);


/*
 * TODO: make a garbage collector thread
 *
 */

void idle()
{
	for(;;);
}

void disableInterrupt()
{
	itEnabled = FALSE;
}
void enableInterrupt()
{
	itEnabled = TRUE;
}

static void initGThreadingSystem()
{
	gThread *mainThread = malloc(sizeof(gThread));
	static struct sigaction sa;
	struct itimerval value;

	mainThread->id = counter++;
	mainThread->next = firstThread;
	mainThread->stack = malloc(STACK_SIZE);
	firstThread = mainThread;
	currentThread = mainThread;


	sigemptyset(&sa.sa_mask);
	sa.sa_handler = yield;
	sa.sa_flags = SA_RESTART | SA_NODEFER;
	sigaction(SIGALRM, &sa, (struct sigaction *)0);
	value.it_interval.tv_sec = SWITCH_LAPSE_SEC;
	value.it_interval.tv_usec = SWITCH_LAPSE_MILLI*1000;
	value.it_value = value.it_interval;
	setitimer(ITIMER_REAL, &value, (struct itimerval *)0);

	createGThread(&idle,NULL,100);
}

int createGThread(void (*sf_addr)(void*),void *sf_arg, int stackSize)
{

	gThread *newThread = malloc(sizeof(gThread));
	itEnabled = FALSE;
	if (counter == 0)
	{
		initGThreadingSystem();
	}
	if(stackSize < STACK_SIZE)
	{
		stackSize = STACK_SIZE;
	}
	newThread->id = counter++;
	newThread->stack = malloc(stackSize);
	mctx_create(&(newThread->context), sf_addr,sf_arg, newThread->stack, STACK_SIZE);
	newThread->next = firstThread;
	firstThread = newThread;
	itEnabled = TRUE;
	return newThread->id;
}

void yield()
{
	gThread* old;
	gThread* toDeletion;
	gThread* iter;
	gThread* iter2;
	gThread* next;
	iter = threadInWaitingState ;
	while (iter != NULL)
	{
		iter->timeToWait -= (SWITCH_LAPSE_SEC*1000+SWITCH_LAPSE_MILLI );
		if (iter->timeToWait <= 0)
		{
			next = iter->next;
			iter->next = firstThread;
			firstThread = iter;
			if (iter == threadInWaitingState)
			{
				threadInWaitingState = next;
			}
			else
			{
				iter2 = threadInWaitingState;
				while(iter2->next!=iter)
				{
					iter2 = iter2->next;
				}
				iter2->next = next;
			}
			iter = next;
		}
		else
		{
			iter = iter->next;
		}
	}
	while (threadForDeletion != NULL)
	{
		toDeletion = threadForDeletion;
		threadForDeletion = threadForDeletion->next;
		free(toDeletion->stack);
		free(toDeletion);
	}
	if (itEnabled == TRUE)
	{
		if(currentThread->context.toDelete)
	    	{
			exitCurrentThread();
		}
		old = currentThread;
		if (currentThread->next == NULL)
		{
			currentThread = firstThread;
		}
		else
		{
			currentThread = currentThread->next;
		}
		mctx_switch(&(old->context),&(currentThread->context));
		enableInterrupt();
	}
}

int killThreadById(int id)
{
	gThread* iter;
	iter = firstThread;
	/* on ne peut pas tuer le thread main */
	if (id == 0)
	{
		return -1;
	}
	while (iter->id != id)
	{
		iter = iter->next;
		if (iter == NULL)
		{
			return -1;
		}
	}
	disableInterrupt();
	removeGThreadFromActivable(iter);
	free(iter->stack);
	free(iter);
	enableInterrupt();
	return 1;


}

static int removeGThreadFromActivable(gThread* toRemove)
{
	gThread* iter = firstThread;
	if (toRemove == firstThread)
	{
		firstThread = firstThread->next;
		if (currentThread == toRemove)
		{
			currentThread = firstThread;
		}
		enableInterrupt();
		return 1;
	}
	else
	{
		while (iter->next != toRemove)
		{
			iter = iter->next;
			if (iter->next == NULL)
			{
			enableInterrupt();
				return -1;
			}
		}
		iter->next = toRemove->next;
		if (currentThread == toRemove)
		{
			currentThread = iter->next;
			if (currentThread == NULL)
			{
				currentThread = firstThread;
			}
		}
	}
	return 1;
}

void sleepMS(int time)
{
	gThread* tmp;
	disableInterrupt();
	currentThread->timeToWait = time;
	tmp = currentThread;
	removeGThreadFromActivable(tmp);
	tmp->next = threadInWaitingState;
	threadInWaitingState = tmp;
	enableInterrupt();
	mctx_switch(&(tmp->context),&(currentThread->context));
}


void exitCurrentThread()
{
	gThread* save;
	disableInterrupt();
	save = currentThread;
	removeGThreadFromActivable(currentThread);
	save->next = threadForDeletion;
	threadForDeletion = save;
	enableInterrupt();
	mctx_restore(&(currentThread->context));
}
