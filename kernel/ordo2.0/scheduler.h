#ifndef SCHEDULER_H_
#define SCHEDULER_H_

#include "gThread.h"
/* un pile d'un mega */
#define STACK_SIZE 1048576


typedef struct thread
{
	struct thread *next;
	mctx_t context;
	int id;
	char stack[STACK_SIZE];
}thread;

void createNewThread(void (*sf_addr)(void*),void *sf_arg);
void yield();
void disableInterrupt();
void enableInterrupt();

#endif /* SCHEDULER_H_ */
