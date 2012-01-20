#include "scheduler.h"

void child()
{
	int i;
	for (i=0;i<3;i++)
	{
		printf("child\n");
		gSleep(1);
	}
    exitCurrentThread();
}
void child2()
{
	int i;
	for (i=0;i< 5;i++)
	{
		printf("child2\n");
		gSleep(1);
	}
    exitCurrentThread();
}

int main()
{
	int i;
	/*THREAD_ID firstThread =*/createGThread(&child,NULL, 0);
	/*THREAD_ID secondThread = createGThread(&child2,NULL, 0);*/
	for (i = 0;; i++)
	{
		printf("main\n");
        gSleep(1);
        exitCurrentThread();
       /* 
		if (i==12)
		{
			killThreadById(firstThread);
		}
        */
	}
	return 1;
}
