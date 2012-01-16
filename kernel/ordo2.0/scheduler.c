#include "scheduler.h"

/* Time of IdleLapse, Float or Int in Seconds */
#define IDLE_LAPSE 0.1

/* Time for RoundRobin, sec in int and milli in int */
#define SWITCH_LAPSE_SEC 1
#define SWITCH_LAPSE_MILLI 0

static thread *firstThread = NULL;
static thread *currentThread = NULL;
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

static void initThreadingSystem()
{
	thread *mainThread = malloc(sizeof(thread));
	static struct sigaction sa;
	struct itimerval value;

	mainThread->id = counter++;
	mainThread->next = firstThread;
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

void createNewThread(void (*sf_addr)(void*),void *sf_arg)
{

	thread *newThread = malloc(sizeof(thread));
	if (counter == 0)
	{
		initThreadingSystem();
	}

	newThread->id = counter++;
	mctx_create(&(newThread->context), sf_addr,sf_arg, newThread->stack, STACK_SIZE);
	newThread->next = firstThread;
	firstThread = newThread;
	itEnabled = TRUE;
}

void yield()
{
	thread* old;
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
	}
}





