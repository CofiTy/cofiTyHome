#include "scheduler.h"


static thread *firstThread = NULL;
static thread *currentThread = NULL;
static int counter = 0;

static void initThreadingSystem()
{
	thread *mainThread = malloc(sizeof(thread));
	mainThread->id = counter++;
	mainThread->next = firstThread;
	firstThread = mainThread;
	currentThread = mainThread;

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
}

void yield()
{
	thread* old;
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





