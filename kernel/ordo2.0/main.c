#include "scheduler.h"



void child()
{
	for (;;)
	{
		printf("child\n");
		sleep(1);
	}
	/*exitCurrentThread();*/
}
void child2()
{
	for (;;)
	{
		printf("child2\n");
		sleep(1);
	}
	/*exitCurrentThread();*/
}

int main()
{
  createGThread(&child,NULL, 0);
  createGThread(&child2,NULL, 0);
	for (;;)
	{
		printf("main\n");
		sleep(1);
	}
	return 1;
}
