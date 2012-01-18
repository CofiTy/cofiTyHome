#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <sys/time.h>
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
 * Can switch context or not?
 */
static volatile int irqEnabled;

/**
 * Enable Timer (and context switching).
 */
void irqEnable()
{
    irqEnabled = 1;
}

/**
 * Disable Timer (and context switching).
 */
void irqDisable()
{
    irqEnabled = 0;
}

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

    irqEnable();
    
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
    gThread *new;
    
    irqDisable();

    new = malloc(sizeof(gThread));
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

    irqEnable();
}

/**
 * Launch the first gThread in the list.
 */
static void launchGThreads()
{
    /* Timer */

    static struct sigaction sa;
    struct itimerval value;
	
    sigemptyset(&sa.sa_mask);
    sa.sa_handler = yield;
    sa.sa_flags = SA_RESTART | SA_NODEFER;
    sigaction(SIGALRM, &sa, (struct sigaction *)0);
    value.it_interval.tv_sec = SWITCH_LAPSE_SEC;
    value.it_interval.tv_usec = SWITCH_LAPSE_MILLI*1000;
    value.it_value = value.it_interval; 
    setitimer(ITIMER_REAL, &value, (struct itimerval *)0);
    
    currentThread = firstThread;
    /*timer_settime (timerid, 0, &value, NULL);*/
    longjmp(currentThread->ctx,1);
}

/**
 * Stop the current thread, save its context
 * and call the next one in the list.
 */
void yield()
{
    if(irqEnabled)
    {
        if (saveCtx(currentThread)==0)
	{
	  /*printf("Timer Tick\n");*/
            if(currentThread->next != NULL) 
              currentThread = currentThread->next;
	    else
	      currentThread = firstThread;
	    longjmp(currentThread->ctx,1);
	}
    }
   /* else
    {
      printf("Timer Tick\n");
    }
*/
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
    if (setjmp(task->ctx))
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
 * Stop and get out the current thread.
 */
void exitCurrentThread()
{
  
  gThread *prev;
  irqDisable();
  if(firstThread == NULL || currentThread == NULL)
    return;

  if(firstThread->id == currentThread->id)
  {
    firstThread = currentThread->next;
    /*free(currentThread->stack);*/
    free(currentThread);
    currentThread = firstThread;
  }
  else
  {
    prev = firstThread->next;
    for(;;)
    {
      printf("Machin %s\n", prev->name);
      if(prev->next == NULL)
	prev = firstThread;
      else
	prev = prev->next;
      if(prev->id != currentThread->id)
	break;
      sleep(1);
    }
    printf("Je sors!\n");
    if(currentThread->next == NULL)
      prev->next = firstThread;
    else
      prev->next = currentThread->next;
    /*free(currentThread->stack);*/
    free(currentThread);
    /* We don't get prev->next,
     * cause in yield we take the next!
     */
    currentThread = prev->next;
  }
  irqEnable();
  yield();
}
