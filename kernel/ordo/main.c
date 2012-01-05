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
    createGThread("process 1", &child);
    createGThread("process 2", &child2);
    launchGThreads();
    return 1;
}

void child()
{
    for (;;)
    {
        printf("%s\n", getCurrentThreadName());
        yield();
    }
}

void mess()
{
    printf("\t%s in mess\n", getCurrentThreadName());
    yield();
}

void child2()
{
    for (;;)
    {
        printf("%s before mess\n", getCurrentThreadName());
        mess();
        printf("%s after mess\n", getCurrentThreadName());
    }
}
