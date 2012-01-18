#include "scheduler.h"

/* Time of IdleLapse, Float or Int in Seconds */
#define IDLE_LAPSE 0.1

/* Time for RoundRobin, sec in int and milli in int */
#define SWITCH_LAPSE_SEC 0
#define SWITCH_LAPSE_MILLI 1

typedef struct gThread
{
	struct gThread *next;
	mctx_t context;
	int id;
	char stack[STACK_SIZE];
} gThread;

static gThread *firstThread = NULL;
static gThread *currentThread = NULL;
static gThread *threadForDeletion = NULL;
static int counter = 0;
volatile int itEnabled = FALSE;

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
	/*mainThread->stack = malloc(STACK_SIZE);*/
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

}

void createGThread(void (*sf_addr)(void*),void *sf_arg, int stackSize)
{

	gThread *newThread = malloc(sizeof(gThread));
	if (counter == 0)
	{
		initGThreadingSystem();
	}

	/*if(stackSize < STACK_SIZE)
	  stackSize = STACK_SIZE;*/
	newThread->id = counter++;
	mctx_create(&(newThread->context), sf_addr,sf_arg, newThread->stack, STACK_SIZE);
	newThread->next = firstThread;
	/*newThread->stack = malloc(stackSize);*/
	firstThread = newThread;
	itEnabled = TRUE;
	
}

void yield()
{
	gThread* old;
	gThread* toDeletion;
	while (threadForDeletion != NULL)
	{
		toDeletion = threadForDeletion;
		threadForDeletion = threadForDeletion->next;
		/*free(toDeletion->stack);*/
		free(toDeletion);
	}
	if (itEnabled == TRUE)
	{
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

int removeGThreadFromActivable(gThread* toRemove)
{
	gThread* iter = firstThread;
	disableInterrupt();
	if (toRemove == firstThread)
	{
		firstThread = firstThread->next;
		if (currentThread == toRemove)
		{
			currentThread = firstThread;

		}
		return 1;
	}
	else
	{
		while (iter->next != toRemove)
		{
			iter = iter->next;
			if (iter->next == NULL)
			{
				return -1;
			}

		}
		iter->next = toRemove->next;
		if (currentThread == toRemove)
		{
			currentThread = iter->next;
		}
		
	}
}

void exitCurrentThread()
{
	gThread* old = currentThread;
	gThread* iter = firstThread;
	disableInterrupt();
	if (currentThread == firstThread)
	{
		firstThread = firstThread->next;
		currentThread = firstThread;
	}
	else
	{
		while (iter->next != old)
		{
			if (iter->next == NULL)
			{
				iter = firstThread;
			}
			else
			{
				iter = iter->next;
			}
		}
		iter->next = old->next;
		if (old->next == NULL)
		{
			currentThread = firstThread;
		}
		else
		{
			currentThread = old->next;
		}
	}
	old->next = threadForDeletion;
	threadForDeletion = old;
	mctx_restore(&(currentThread->context));
}

