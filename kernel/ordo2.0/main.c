#include "scheduler.h"

semaphore *sem1;
messageQueue *mq1;

void child()
{
	int i;
	char buffer[20];
	for (i=0;;i++)
	{
		receiveMessage(mq1,buffer);
		printf("child: %s\n",buffer);
		/*gSleep(5);*/
	}
}
void child2()
{
	int i;
	char buffer[20];
	for (i=0;;i++)
	{
		/*semTake(sem1);*/
		printf("child2 : %s\n",buffer);
		/*gSleep(1);*/
	}
}

int main()
{
	int i;
	char buffer[20];
	createGThread(&child,NULL, 0, "child");
	/*createGThread(&child2,NULL, 0);*/
/*	sem1 = newSemaphore(3);*/
	mq1 = newMessageQueue();
	for (i=0;;i++)
	{
		printf("main\n");
		gSleep(3);
		if ((i%2) == 0)
		{
			sprintf(buffer,"Hello : %d\n",i);
			sendMessage(mq1,buffer,20);
			/*semGive(sem1);*/
		}
	}
	return 1;
}
