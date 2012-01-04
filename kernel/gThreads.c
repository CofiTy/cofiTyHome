#include <setjmp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <time.h>
#include <unistd.h>

#include "gThreads.h"

static int id_counter = 0;
void **ebp;
gThread *thread = NULL;

void child();
void child2();
static int saveCtx(gThread* task);

void createGThread(void(*function)(void))
{
    
    gThread *new = malloc(sizeof(gThread));
    new->id = ++id_counter;
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

static int saveCtx(gThread* task)
{
    int i;
    void **esp;
    asm("mov %%esp, %0" : "=r" (esp));
    task->stackSize = (int)(ebp - esp);
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

void yield()
{
    if (saveCtx(thread)==0)
    {
        thread = thread->next; 
        longjmp(thread->ctx,1);
    }
}

void initGThreadingSystem()
{
    int timer;
    struct sigaction sa;
    struct sigevent sev;
    asm("mov %%ebp, %0" : "=r" (ebp));
    thread=NULL;
  //  sa.sa_flags = SA_SIGINFO;
  //  sa.sa_sigaction = yield;
  //  sigemptyset(&sa.sa_mask);
  //  sev.sigev_notify = SIGEV_SIGNAL;
  //  sev.sigev_signo = 15;
  //  sev.sigev_value.sival_ptr = &timer;
  //  timer_create(CLOCK_REALTIME, &sev, &timerid);
    
}

void launchGThreads()
{
    longjmp(thread->ctx,1);
}

