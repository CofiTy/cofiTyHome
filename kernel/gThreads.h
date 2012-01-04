#ifndef kernel_myThreads_h
#define kernel_myThreads_h

#include <setjmp.h>

/**
 * Librairie de création de threads pour gHome.
 *
 * TODO: Rendre le système préemptif pour implémenter
 * un round robin.
 */

/*
 * Structure d'un thread gHome.
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

/*
 * Initialise le système de threading.
 */
void initGThreadingSystem();

/*
 * Créer un nouveau gThread.
 */
void createGThread(void(*function)(void));

/*
 * Lance les gThreads.
 */
void launchGThreads();

/*
 * Redonne la main au prochain thread
 * dans le cas d'un système collaboratif.
 */
void yield();

#endif
