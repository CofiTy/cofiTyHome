#include "scheduler.h"



void child()
{
	for (;;)
	{
		printf("child\n");
		sleep(1);
		yield();
	}
}

int main()
{
	createNewThread(&child,NULL);
	for (;;)
	{
		printf("main\n");
		sleep(1);
		yield();
	}
	return 1;
}
