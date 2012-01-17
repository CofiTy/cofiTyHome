#include "scheduler.h"



void child()
{
	int i;
	for ( i = 0 ; i < 5 ; i++ )
	{
		printf("child\n");
		sleep(1);
	}
	exitCurrentThread();
}
void child2()
{
	int i;
	for ( i = 0 ; i < 2 ; i++ )
	{
		printf("child2\n");
		sleep(1);
	}
	exitCurrentThread();
}

int main()
{
	createNewThread(&child,NULL);
	createNewThread(&child2,NULL);
	for (;;)
	{
		printf("main\n");
		sleep(1);
	}
	return 1;
}
