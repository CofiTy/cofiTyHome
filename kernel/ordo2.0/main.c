#include "scheduler.h"



void child()
{
	int i;
	for (i=0;i< 2;i++)
	{
		printf("child\n");
		sleep(1);
	}
	/*exitCurrentThread();*/
}
void child2()
{
	int i;
	for (i=0;i< 5;i++)
	{
		printf("child2\n");
		sleep(1);
	}
	/*exitCurrentThread();*/
}

int main()
{
	int i;
  createGThread(&child,NULL, 0);
  createGThread(&child2,NULL, 0);
	for (i=0;i< 10;i++)
	{
		printf("main\n");
		sleep(1);
	}
	return 1;
}
