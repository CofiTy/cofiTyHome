#ifndef kernel_myThreads_h
#define kernel_myThreads_h

#include <setjmp.h>

/**
 * Thread API For gHome.
 *
 * TODO: Make a roundrobin system instead
 * of a collaborative one.
 */

/**
 * gHome simplified thread's structure.
 */ 
typedef struct gThread
{  
    int id;
    int priority;
    void* stack[200];
    int stackSize;
    jmp_buf ctx;
    struct gThread *next;
} gThread;

/**
 * Threading system initialisation.
 */
void initGThreadingSystem();

/**
 * New gThread creation.
 */
void createGThread(void(*function)(void));

/**
 * Launch the gThreads.
 */
void launchGThreads();

/**
 * Activate the next gThread (collaborative function).
 */
void yield();

#endif
