#include <setjmp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <time.h>
#include <unistd.h>
#include "myThreads.h"



static int id_counter = 0;
void **rbp;
Thread *thread = NULL;

void child();
void child2();
static int save_ctx(Thread* task);

void createThread(void(*function)(void))
{
    
    Thread *new = malloc(sizeof(Thread));
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
    if (save_ctx(new))
    {
        function();
    }
    
}

static int save_ctx(Thread* task)
{
    int i;
    void **rsp;
    asm("mov %%rsp, %0" : "=r" (rsp));
    task->stack_size = (int)(rbp - rsp);
    for (i=0;i<task->stack_size;i++)
    {
        task->stack[i] = rbp[-i];
    }
    if (setjmp(task->ctx))
    {
        for (i=0;i<thread->stack_size;i++)
        {
            *(rbp-i) = thread->stack[i];
        }
        return 1;
    }
    return 0;
}

void yield()
{
    if (save_ctx(thread)==0)
    {
        thread = thread->next; 
        longjmp(thread->ctx,1);
    }
}

void init_threading_system()
{
    int timer;
    struct sigaction sa;
    struct sigevent sev;
    asm("mov %%rbp, %0" : "=r" (rbp));
    thread=NULL;
  //  sa.sa_flags = SA_SIGINFO;
  //  sa.sa_sigaction = yield;
  //  sigemptyset(&sa.sa_mask);
  //  sev.sigev_notify = SIGEV_SIGNAL;
  //  sev.sigev_signo = 15;
  //  sev.sigev_value.sival_ptr = &timer;
  //  timer_create(CLOCK_REALTIME, &sev, &timerid);
    
}

void launch_threads()
{
    longjmp(thread->ctx,1);
}

