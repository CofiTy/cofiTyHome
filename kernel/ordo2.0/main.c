#include "scheduler.h"

semaphore *sem1;

void child()
{
	int i;
	for (i=0;i<3;i++)
	{
		printf("child\n");
		gSleep(1);
	}
}
void child2()
{
	int i;
	for (i=0;;i++)
	{
		semTake(sem1);
		printf("child2 a pris un jeton\n");
		/*gSleep(1);*/
	}
}

int main()
{
	int i;
	/*THREAD_ID firstThread =*/createGThread(&child2,NULL, 0);
	/*THREAD_ID secondThread = */createGThread(&child2,NULL, 0);
	sem1 = newSemaphore(3);
	for (i=0;;i++)
	{
		printf("main\n");
		gSleep(1);
		if ((i%2) == 0)
		{
			semGive(sem1);
		}
	}
	return 1;
}
