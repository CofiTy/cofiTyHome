#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <time.h>
#include <unistd.h>

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
gThread *thread;

/**
 * Our function to save the context,
 * not to be used by the programmer...
 */
static int saveCtx(gThread* task);

/**
 * Initialise the threading
 * by getting the current base pointer
 * and setting the thread list to NULL.
 */
void initGThreadingSystem()
{
    asm("mov %%ebp, %0" : "=r" (ebp));
    thread=NULL;
    /*
    int timer;
    struct sigaction sa;
    struct sigevent sev;
    sa.sa_flags = SA_SIGINFO;
    sa.sa_sigaction = yield;
    sigemptyset(&sa.sa_mask);
    sev.sigev_notify = SIGEV_SIGNAL;
    sev.sigev_signo = 15;
    sev.sigev_value.sival_ptr = &timer;
    timer_create(CLOCK_REALTIME, &sev, &timerid);
    */
}

/**
 * Create (allocate) a new gThread,
 * add it to the list,
 * create it a context and
 * and later used to call its function.
 */
void createGThread(char *name, void(*function)(void))
{
    gThread *new = malloc(sizeof(gThread));
    memset(new, 0, sizeof(gThread));
    new->id = ++id_counter;
    memcpy(new->name, name, NAME_SIZE);
    if (thread==NULL)
    {
        new->next = new;
    }
    else
    {
        new->next = thread;
        thread->next = new;
    }
    thread = new;
    if (saveCtx(new))
    {
        function();
    }
}

/**
 * Launch the first gThread in the list.
 */
void launchGThreads()
{
    longjmp(thread->ctx,1);
}

/**
 * Stop the current thread, save its context
 * and call the next one in the list.
 */
void yield()
{
    if (saveCtx(thread)==0)
    {
        thread = thread->next; 
        longjmp(thread->ctx,1);
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
    
    if((task->stackSize = (int)(ebp - esp)) >= STACK_SIZE)
    {
        exit(ERROR_STACK_OVERFLOW);
    }
    
    for (i=0;i<task->stackSize;i++)
    {
        task->stack[i] = ebp[-i];
    }
    if (setjmp(task->ctx))
    {
        for (i=0;i<thread->stackSize;i++)
        {
            *(ebp-i) = thread->stack[i];
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
    return thread->name;
}




