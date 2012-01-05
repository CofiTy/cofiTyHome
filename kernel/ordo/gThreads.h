#ifndef _GTHREADS_H_
#define _GTHREADS_H_

#include <setjmp.h>

#define ERROR_STACK_OVERFLOW -42

#define STACK_SIZE 200
#define NAME_SIZE 16

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
    char name[NAME_SIZE];
    int priority;
    void* stack[STACK_SIZE];
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
void createGThread(char *name, void(*function)(void));

/**
 * Launch the gThreads.
 */
void launchGThreads();

/**
 * Activate the next gThread (collaborative function).
 */
void yield();

/**
 * Return the name of the running process.
 */
char * getCurrentThreadName();

#endif
