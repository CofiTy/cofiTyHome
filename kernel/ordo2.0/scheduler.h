#ifndef SCHEDULER_H_
#define SCHEDULER_H_

#include "gThread.h"

/**
 * Default 1 Mo Stack.
 */
#define STACK_SIZE 1048576

/**
 * Convenient type.
 */
#define THREAD_ID int

/** 
 * Time for RoundRobin, 
 * sec in int and milli in int 
 */
#define SWITCH_LAPSE_SEC 0
#define SWITCH_LAPSE_MILLI 10

/**
 * Function returns, convenient.
 */
#define ERROR -1
#define OK 1

/**
 * Launch a new thread.
 *
 * Args:
 *   1: function to launch.
 *   2: Args for the function.
 *   3: Stack size, STACK_SIZE if 0
 *
 * Return:
 *   Id of the launched thread.
 */
THREAD_ID createGThread(void (*sf_addr)(void*),void *sf_arg, int stackSize);

/**
 * Kill a thread (Sleeping or Activable).
 *
 * Args:
 *   1: Id of the thread to kill.
 *
 * Return:
 *   OK or ERROR.
 */
int killThreadById(THREAD_ID id);

/**
 * Make the current thread to sleep.
 *
 * Args:
 *   1: Nb Seconds to sleep.
 *
 * Note:
 *   If the round robin time is higher
 *   than "seconds", the thread will
 *   be woken up in the lowest multiple
 *   of round robin time higher than seconds.
 */
void gSleep(int seconds);

/** 
 * Function to exit the current thread.
 *
 * Doesn't work for main thread.
 *
 * Useless: A return will do the same.
 */
void exitCurrentThread();

#endif /* SCHEDULER_H_ */
