#include <time.h>
#include <unistd.h>

#include "scheduler.h"

/**
 * Private thread structure (linked list).
 */

/**
 * Pointer to the firstThread of the Activable List.
 */
static gThread *firstThread;

/**
 * Running Thread.
 */
static gThread *currentThread = NULL;

/**
 * List of ended threads to delete.
 */
static gThread *threadForDeletion = NULL;

/**
 * List of sleeping threads.
 */
static gThread *threadInWaitingState = NULL;

/**
 * Thread Id Counter.
 */
static THREAD_ID counter = -1;

/**
 * Tell if interrupts are enabled.
 */
volatile int itEnabled = FALSE;

/**
 * Garbage Collection Thread,
 * Remove killed or stopped threads.
 */
void gc()
/*
 * TODO: why don't put a wait in this function to increase
 * performances?
 *
 */
{
	for(;;)
	{
		while (threadForDeletion != NULL)
		{
			gThread* toDeletion = threadForDeletion;
			threadForDeletion = threadForDeletion->next;
			free(toDeletion->stack);
			free(toDeletion);
		}
	}
}

/**
 * Idle Thread, used
 * to get more than one thread.
 * TODO: I think it's no useful anymore...
 */
void idle()
{
	for(;;);
}

/**
 * Remove a given thread from waiting thread list.
 * TODO: pourquoi tu fait un test si le thread est le currentThread
 * c'est impossible? non
 */
static int removeGThreadFromWaiting(gThread* toRemove)
{
	gThread* iter = threadInWaitingState;
	if (toRemove == threadInWaitingState)
	{
		threadInWaitingState = threadInWaitingState->next;
		if (currentThread == toRemove)
		{
			currentThread = threadInWaitingState;
		}
		return OK;
	}
	else
	{
		while (iter->next != toRemove)
		{
			iter = iter->next;
			if (iter->next == NULL)
			{
				return ERROR;
			}
		}
		iter->next = toRemove->next;
		if (currentThread == toRemove)
		{
			currentThread = iter->next;
			if (currentThread == NULL)
			{
				currentThread = threadInWaitingState;
			}
		}
	}
	return OK;
}

/**
 * Remove a given thread from activable thread list.
 */
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
		return OK;
	}
	else
	{
		while (iter->next != toRemove)
		{
			iter = iter->next;
			if (iter->next == NULL)
			{
				return ERROR;
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
	return OK;
}

/**
 * Disable Interrupts.
 * Basically, its don't do the yield
 * at each clock top.
 */
void disableInterrupt()
{
	itEnabled = FALSE;
}

/**
 * Enable Interrupts.
 */
void enableInterrupt()
{
	itEnabled = TRUE;
}

/**
 * Initialise the threading system by
 * creating a main, gc and idle thread.
 */
static void initGThreadingSystem()
{
	gThread *mainThread = malloc(sizeof(gThread));
	static struct sigaction sa;
	struct itimerval value;

	firstThread = NULL;
	mainThread->id = counter++;
	mainThread->next = firstThread;
	mainThread->stack = malloc(STACK_SIZE);
	mainThread->name[0] = 'm';
	mainThread->name[1] = 'a';
	mainThread->name[2] = 'i';
	mainThread->name[3] = 'n';
	mainThread->name[4] = '\0';

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

	createGThread(&idle,NULL,100,"idle");
	createGThread(&gc, NULL, 0,"gc");
}

/**
 * Create a new thread, 
 * initialise the threading system if not done before.
 */
THREAD_ID createGThread(void (*sf_addr)(void*),void *sf_arg, int stackSize, char* name)
{

	int i;
	gThread *newThread;
	disableInterrupt();
	if (counter++ == -1)
	{
		initGThreadingSystem();
	}
	if(stackSize < STACK_SIZE)
	{
		stackSize = STACK_SIZE;
	}
	newThread = malloc(sizeof(gThread));
	for (i=0; i<20;i++)
	{
		newThread->name[i] = name[i];
	}

	newThread->id = counter++;
	newThread->stack = malloc(stackSize);
	mctx_create(&(newThread->context), sf_addr,sf_arg, newThread->stack, STACK_SIZE);
	newThread->next = firstThread;
	firstThread = newThread;
	itEnabled = TRUE;
	enableInterrupt();
	return newThread->id;
}

/**
 * If interrupts are enabled,
 * try to wake up the sleeping threads,
 * switch context with the next thread of the
 * activable list.
 * TODO: tu fais une boucle dans laquelle tu appelles la fonction time qui est
 * une fonction qui fait un appel système (lourd) tu pourrais le faire juste une
 * foi avant le boucle.
 */
void yield()
{
	gThread *old;
	if (itEnabled == TRUE)
	{
		while(threadInWaitingState != NULL
		&& threadInWaitingState->timeToWait <= time(NULL))
		{
			gThread *threadToWakeUp = threadInWaitingState;
			removeGThreadFromWaiting(threadToWakeUp);            
			threadToWakeUp->next = firstThread;
			firstThread = threadToWakeUp;
		}
		if(currentThread->context.toDelete)
		{
			exitCurrentThread();
		}
		old = currentThread;
		currentThread = currentThread->next;
		if (currentThread == NULL)
		{
			currentThread = firstThread;
		}
		mctx_switch(&(old->context),&(currentThread->context));
		enableInterrupt();
	}
}

/**
 * Kill a thread by its id.
 * Try to remove it from the activable list,
 * and then try the waiting list.
 */
int killThreadById(THREAD_ID id)
{
	gThread* iter;
	disableInterrupt();
	/* Cannot Kill main, idle and gc thread,
	* the first three ones.
	*/
	if (id == 0 || id == 1 || id ==2 )
	{
		enableInterrupt();
		return ERROR;
	}

	iter = firstThread;
	while (iter != NULL && iter->id != id)
	{
		iter = iter->next;
	}
	if(iter == NULL)
	{
		enableInterrupt();
		return ERROR;
	}
	if(removeGThreadFromActivable(iter) != OK)
	{
		removeGThreadFromWaiting(iter);
	}
	free(iter->stack);
	free(iter);
	enableInterrupt();
	return OK;
}

/**
 * Put a thread in the waiting list for given amount of seconds.
 */
void gSleep(int seconds)
{
	gThread *toSleep, *iter, *prev;
	disableInterrupt();

	if(counter == 0)
	{
		initGThreadingSystem();
	}

	currentThread->timeToWait = time(NULL) + seconds;
    
	prev = NULL;
	iter = threadInWaitingState;
	while(iter != NULL
		&& iter->timeToWait < currentThread->timeToWait)
	{
		prev = iter;
		iter = iter->next;
	}

	toSleep = currentThread;

	removeGThreadFromActivable(currentThread);
	toSleep->next = iter;

	if(prev == NULL)
	{
		threadInWaitingState = toSleep;
	}
	else
	{
		prev->next = toSleep;
	}

	enableInterrupt();
	mctx_switch(&(toSleep->context),&(currentThread->context));
}

/**
 * Exit current thread, except if threading not initialised,
 * and if were in the main/gc/idle thread.
 *
 * This function is optionnal, a return will do the same thing.
 */
void exitCurrentThread()
{
	gThread *save;
	if(currentThread == NULL)
	{
		return;
	}
	if(currentThread->id == 0
		|| currentThread->id == 1
		|| currentThread->id == 2)
	{
		return;
	}
	disableInterrupt();
	save = currentThread;
	removeGThreadFromActivable(currentThread);
	save->next = threadForDeletion;
	threadForDeletion = save;
	enableInterrupt();
	mctx_restore(&(currentThread->context));
}


semaphore* newSemaphore(int init)
{
	semaphore *sem = malloc(sizeof(semaphore));
	sem->counter = init;
	sem->inWait = NULL;
	return sem;
}

int deleteSemaphore(semaphore* sem)
{
	if (sem->inWait != NULL)
	{
		return -1;
	}
	free(sem);
	return 1;
}


void semTake(semaphore *sem)
{
	gThread *save;
	disableInterrupt();
	if (sem->counter == 0)
	{
		save = currentThread;
		removeGThreadFromActivable(currentThread);
		save->next = sem->inWait;
		sem->inWait = save;
		enableInterrupt();
		mctx_switch(&(save->context),&(currentThread->context));
	}
	else
	{
	sem->counter--;
	}
}

void semGive(semaphore *sem)
{
	gThread *save;
	disableInterrupt();
	if (sem->inWait != NULL)
	{
		save = sem->inWait;;
		sem->inWait = sem->inWait->next;
		save->next = firstThread;
		firstThread = save;
	}
	else
	{
		sem->counter++;
	}
	enableInterrupt();
}



messageQueue *newMessageQueue()
{
	messageQueue *new;
	disableInterrupt();
	new = malloc(sizeof(messageQueue));
	new->inBox = NULL;
	new->inWait = NULL;
	enableInterrupt();
	return new;
}

int receiveMessage(messageQueue *mQ, char* buffer)
{
	int i;
	int size;
	gThread *save;
	message *toDelete;
	for(;;)
	{
		disableInterrupt();
		toDelete = mQ->inBox;
		if (toDelete != NULL)
		{
			for (i=0;i<toDelete->size;i++)
			{

				buffer[i] = toDelete->data[i];
			}
			mQ->inBox = toDelete->next;
			size = toDelete->size;
			free(toDelete->data);
			free(toDelete);
			enableInterrupt();
			return size;
		}
		save = currentThread;
		removeGThreadFromActivable(currentThread);
		save->next = mQ->inWait;
		mQ->inWait = save;
		enableInterrupt();
		mctx_switch(&(save->context),&(currentThread->context));
	}
}

void sendMessage(messageQueue *mQ, char* myMessage, int messageSize)
{

	int i;
	message *iter;
	message *new;
	gThread *toRestore;
	disableInterrupt();
	/* If the message size is null,
	 * no message is sent
	 */
	if (messageSize == 0)
	{
		return;
	}
	/* allocating memory for the new message */
	new = malloc(sizeof(message));
	new->data = malloc(messageSize);
	new->size = messageSize;
	new->next = NULL;
	/* Copy the content of the message*/
	for (i=0; i<messageSize;i++)
	{
		new->data[i] = myMessage[i];
	}
	if (mQ->inBox == NULL)
	{
		mQ->inBox = new;
	}
	else
	{
		for(iter=mQ->inBox;iter->next!=NULL;iter=iter->next);
		iter->next = new;
	}
	if (mQ->inWait!=NULL)
	{
		toRestore = mQ->inWait;
		mQ->inWait = mQ->inWait->next;
		toRestore->next = firstThread;
		firstThread = toRestore;
	}
	enableInterrupt();
	return;
}




