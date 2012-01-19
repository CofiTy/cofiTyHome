#include "scheduler.h"



void child()
{
	int i;
	for (i=0;i< 222;i++)
	{
		printf("child\n");
		sleep(1);
	}
}
void child2()
{
	int i;
	for (i=0;i< 5;i++)
	{
		printf("child2\n");
		sleep(1);
	}
}

int main()
{
	int i;
	int firstThread = createGThread(&child,NULL, 0);
	int secondThread = createGThread(&child2,NULL, 0);
	for (i=0;i< 10;i++)
	{
		printf("main\n");
		sleep(1);
		if (i==2)
		{
			killThreadById(firstThread);
		}
	}
	return 1;
}
