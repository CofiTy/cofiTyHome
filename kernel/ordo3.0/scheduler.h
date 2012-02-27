#ifndef SCHEDULER_H
#define SCHEDULER_H
#include "context.h"
#include "m_queue.h"
#include <stdlib.h>
#include <stdio.h>
#include "interrupt.h"

/*
 * Library for using multithreading systeme
 */

/*
 * the function type to use in threading system
 */
typedef void(fct2)(void);

/*
 * Schedule threads
 * parameter:
 * 	to_save: the context to save before switching
 */
void scheduler(ctx* to_save);

/*
 * Spawn a new thread
 * parameters:
 * 	stack_size: stack size to allocate
 * 	func: the thread function
 * return value: the new thread id
 */
int create_thread(int stack_size, fct2 *func);

/*
 * Start the threading system
 */
void start_scheduler();

/*
 * wait for a deleted thread
 * return value: the id of the deleted thread
 */
int wait();

/*
 * suspend the current thread
 * parameter:
 * 	seconds: time to sleep in second
 */
void sleep(int seconds);

/*
 * Remove the current thread from the activable threads
 */
void disable_current_thread();

/*
 * linked list to sleeping threads
 */
ctx *thread_in_waiting;

/*
 * The active thread
 */
ctx *current_thread;

/*
 * thread table (index is the thread id)
 */
ctx *threads[256];


#endif
