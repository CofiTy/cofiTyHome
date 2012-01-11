#include <stdio.h>
#include <signal.h>

#include <unistd.h>

#include "gThreads.h"

void child();
void child2();
void child3();
void child4();
void mess();

/**
 * Procédure de test de l'ordonnanceur.
 */

int main()
{
    initGThreadingSystem();
    createGThread("process 1", &child);
    createGThread("process 2", &child2);
    createGThread("process 3", &child3);
    for(;;)
    {
      printf("main\n");
      sleep(1);
      /*yield();*/
    }
    return 1;
}

void child4()
{
  for(;;)
  {
    printf("Voila voila %s\n", getCurrentThreadName());
    sleep(1);
    /*yield();*/
  }
}

void child3()
{
  createGThread("process 4", &child4);
  for(;;)
  {
      printf("Pas mal %s!\n", getCurrentThreadName());
      sleep(1);
	/*yield();*/
  }
}

void child()
{
    for (;;)
    {
        printf("Yeahhh %s\n", getCurrentThreadName());
	sleep(1);
        /*yield();*/
    }
}

void mess()
{
    printf("\t%s in mess\n", getCurrentThreadName());
    sleep(1);
    /*yield();*/
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
