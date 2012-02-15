#ifndef SCHEDULER_H
#define SCHEDULER_H
#include "context.h"
#include "m_queue.h"
#include <stdlib.h>
#include <stdio.h>
#include "interrupt.h"

typedef void(fct2)(void);
void scheduler(ctx* to_save);
int create_thread(int stack_size, fct2 *func);
void start_scheduler();
int wait();
void sleep(int seconds);
void disable_current_thread();
ctx *thread_in_waiting;
ctx *current_thread;
ctx *threads[256];


#endif
