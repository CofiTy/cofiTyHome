#ifndef _GTHREADS_H_
#define _GTHREADS_H_

#include <setjmp.h>

#define ERROR_STACK_OVERFLOW -42

#define STACK_SIZE 200
#define NAME_SIZE 16

/**
 * Thread API For gHome.
 *
 * TODO:
 * -- Rendre le système préemptif.
 * -- Faire quelque chose afin de lancer le thread à sa création.
 * -- Gérer la mort d'un thread, afin de libérer la mémoire.
 * -- Gérer les wait.
 * -- Gérer les bloquages du aux entrées/sortie???
 */

typedef void threadFunc(void);

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
    threadFunc *start;
    struct gThread *next;
} gThread;

/**
 * Threading system initialisation.
 */
void initGThreadingSystem();

/**
 * New gThread creation.
 */
void createGThread(char *name, threadFunc function);

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
