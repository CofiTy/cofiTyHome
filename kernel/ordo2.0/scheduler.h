#ifndef SCHEDULER_H_
#define SCHEDULER_H_

#include "gThread.h"
/* un pile d'un mega */
#define STACK_SIZE 1048576

void createNewThread(void (*sf_addr)(void*),void *sf_arg);
void disableInterrupt();
void enableInterrupt();
/* Function to exit the current thread, do not use to exit the
 * thread
 */
void exitCurrentThread();

#endif /* SCHEDULER_H_ */
