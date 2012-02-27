#ifndef SEMAPHORE_H
#define SEMAPHORE_H
#include "scheduler.h"

/*
 * This library emplements semaphores
 */

/*
 * Create a new semaphore
 * parameter:
 * 	init: initial number of coins
 * return value: semaphore id
 */
int new_semaphore(int init);

/*
 * Give a coin to a semaphore
 * parameter:
 * 	sem_id: semaphore id
 */
void sem_give(int sem_id);

/*
 * Take a coin from a semaphore
 * This function block until a coin is available
 * parameter:
 * 	sem_id: semaphore id
 */
void sem_take(int sem_id);

/*
 * Delete a semaphore
 * parameter:
 * 	id: semaphore id
 * return value: -1 if an error occured, 1 otherwise
 */
int del_semaphore(int id);

#endif
