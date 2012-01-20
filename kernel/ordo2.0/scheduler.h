#ifndef SCHEDULER_H_
#define SCHEDULER_H_

#include "gThread.h"
/* un pile d'un mega */
#define STACK_SIZE 1048576

#define THREAD_ID int

/* Time for RoundRobin, sec in int and milli in int */
#define SWITCH_LAPSE_SEC 0
#define SWITCH_LAPSE_MILLI 10

#define ERROR -1
#define OK 1

THREAD_ID createGThread(void (*sf_addr)(void*),void *sf_arg, int stackSize);

int killThreadById(THREAD_ID id);

void gSleep(int seconds);

/* Function to exit the current thread, do not use to exit the
 * main thread
 */
void exitCurrentThread();

#endif /* SCHEDULER_H_ */
