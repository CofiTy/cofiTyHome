#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <time.h>
#include <unistd.h>
#include <pthread.h>

#include <stdio.h>

#include "gThreads.h"

/**
 * Counter to give id to new threads.
 */
static int id_counter = 0;

/**
 * Current base pointer.
 */
void **ebp;

/**
 * Linked List of gThreads.
 */
gThread *firstThread;

/**
 * Running gThread.
 */
gThread *currentThread;

/**
 * Launch the gThreads.
 */
static void launchGThreads();

/**
 * Our function to save the context,
 * not to be used by the programmer...
 */
static int saveCtx(gThread* task);

/**
 * Idle Thread, do nothing.
 */
static void idleFunc();

/**
 * Initialise the threading
 * by getting the current base pointer
 * and setting the thread list to NULL.
 */
void initGThreadingSystem()
{

    gThread *mainThread;

    /* Threading Init */
    asm("mov %%ebp, %0" : "=r" (ebp));
    firstThread = currentThread = NULL;
    createGThread("idle", &idleFunc);

    /*
     * Création du thread principal
     * On le créé ici car ce n'est pas
     * une fonction et il ne peut donc
     * pas être créé dans createGThread.
     */
    mainThread = malloc(sizeof(gThread));
    memset(mainThread, 0, sizeof(gThread));
    mainThread->id = id_counter++;
    memcpy(mainThread->name, "main", NAME_SIZE);
    mainThread->next = firstThread;
    mainThread->start = NULL;
    firstThread = mainThread;
    
    /* L'appel d'init lance les threads,
     * la restauration de contexte passe.
     */
    if (!saveCtx(mainThread))
    {
       launchGThreads();
    }
   
}

/**
 * Create (allocate) a new gThread,
 * add it to the list,
 * create it a context and
 * and later used to call its function.
 */
void createGThread(char *name, threadFunc function)
{
    gThread *new = malloc(sizeof(gThread));
    memset(new, 0, sizeof(gThread));
    new->id = id_counter++;
    memcpy(new->name, name, NAME_SIZE);
    new->next = firstThread;
    new->start = function;
    firstThread = new;
    
    if (saveCtx(new))
    {
        new->start();
    }
}

/**
 * Launch the first gThread in the list.
 */
static void launchGThreads()
{
    /* Timer */ 
    timer_t timerid;
    struct itimerspec value;

    struct sigevent sev;
    struct sigaction sa;
  
    value.it_value.tv_sec = SWITCH_LAPSE_SEC;
    value.it_value.tv_nsec = SWITCH_LAPSE_MILLI*1000000;
    value.it_interval.tv_sec = SWITCH_LAPSE_SEC;
    value.it_interval.tv_nsec = SWITCH_LAPSE_MILLI*1000000;
    sa.sa_flags = SA_SIGINFO;
    sa.sa_sigaction = yield;
    sigemptyset(&sa.sa_mask);
    sigaction(SIGRTMIN, &sa, NULL);
    sev.sigev_notify = SIGEV_SIGNAL;
    sev.sigev_signo = SIGRTMIN;
    sev.sigev_value.sival_ptr = &timerid;
    timer_create (CLOCK_REALTIME, &sev, &timerid);
    
    currentThread = firstThread;
    timer_settime (timerid, 0, &value, NULL);
    siglongjmp(currentThread->ctx,1);
}

/**
 * Stop the current thread, save its context
 * and call the next one in the list.
 */
void yield()
{
    
    if (saveCtx(currentThread)==0)
    {
      /*printf("Timer Tick\n");*/
        if(currentThread->next != NULL) 
            currentThread = currentThread->next;
	else
	    currentThread = firstThread;
        siglongjmp(currentThread->ctx,1);
    }
}

/**
 * Save the context of a thread.
 * Its stack pointer, and its whole stack.
 */
static int saveCtx(gThread* task)
{
    int i;
    void **esp;
    
    asm("mov %%esp, %0" : "=r" (esp));

    /* Get the current process stack size
     * and exit brutally if to big...
     */
    if((task->stackSize = (int)(ebp - esp)) >= STACK_SIZE)
    {
        exit(ERROR_STACK_OVERFLOW);
    }
   
    /* Copy the current stack size
     * into the process backup stack
     */
    for (i=0;i<task->stackSize;i++)
    {
        task->stack[i] = ebp[-i];
    }

    /* If called by longjmp,
     * restore the stack, and
     * say to saveCtx that we can execute
     * the thread function.
     */
    if (sigsetjmp(task->ctx, 1))
    {
        for (i=0;i<currentThread->stackSize;i++)
        {
            *(ebp-i) = currentThread->stack[i];
        }
        return 1;
    }
    return 0;
}

/**
 * Return the running process name.
 */
char * getCurrentThreadName()
{
    return currentThread->name;
}

/**
 * Idle Thread...
 */
static void idleFunc()
{
    for(;;)
    {
        printf("Idle\n");
        sleep(1);
        /*yield();*/
    }
}



