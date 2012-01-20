#include "scheduler.h"

void child()
{
	int i;
	for (i=0;i< 222;i++)
	{
		printf("child\n");
		gSleepMs(10000);
	}
    exitCurrentThread();
}
void child2()
{
	int i;
	for (i=0;i< 5;i++)
	{
		printf("child2\n");
		gSleepMs(10000);
	}
    exitCurrentThread();
}

int main()
{
	int i;
	int firstThread = createGThread(&child,NULL, 0);
	createGThread(&child2,NULL, 0);
	for (i=0;i< 110;i++)
	{
		printf("main\n");
        gSleepMs(10000);
		if (i==2)
		{
			killThreadById(firstThread);
		}
	}
	return 1;
}
