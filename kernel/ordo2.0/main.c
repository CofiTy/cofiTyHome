#include "scheduler.h"



void child()
{
	for (;;)
	{
		printf("child\n");
		sleep(1);
	}
}
void child2()
{
	for (;;)
	{
		printf("child2\n");
		sleep(1);
	}
}

int main()
{
	createNewThread(&child,NULL);
	createNewThread(&child2,NULL);
	for (;;)
	{
		printf("main\n");
		sleep(1);
	//	yield();
	}
	return 1;
}
