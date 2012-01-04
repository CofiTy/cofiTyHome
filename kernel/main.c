#include <stdio.h>
#include <signal.h>

#include "gThreads.h"

void child();
void child2();
void mess();

/**
 * Procédure de test de l'ordonnanceur.
 */

int main()
{
    initGThreadingSystem();
    createGThread(&child);
    createGThread(&child2);
    launchGThreads();
    return 1;
}

void child()
{
    for (;;)
    {
        printf(" 1");  
        yield();
    }
}

void mess()
{
    printf("\t in mess\n");
    yield();
}

void child2()
{
    for (;;)
    {
        printf("before mess");
        mess();
        printf("after mess");
    }
}
