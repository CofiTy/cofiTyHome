#ifndef SCHEDULER_H_
#define SCHEDULER_H_

#include "gThread.h"
/* un pile d'un mega */
#define STACK_SIZE 1048576

typedef struct gThread
{
	struct gThread *next;
	mctx_t context;
	int id;
	char *stack;
} gThread;

void createGThread(void (*sf_addr)(void*),void *sf_arg, int stackSize);
void disableInterrupt();
void enableInterrupt();
int killThreadById(int id);
/* Function to exit the current thread, do not use to exit the
 * main thread
 */
void exitCurrentThread();

#endif /* SCHEDULER_H_ */
