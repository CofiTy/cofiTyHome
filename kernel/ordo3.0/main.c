#include <stdio.h>
#include <stdlib.h>
#include "hw.h"
#include "scheduler.h"
#include "semaphore.h"
#include "m_queue.h"

/*
 * TODO: CONTEXT DUPLICATION
 */

void pam();
void ping();
void pong();
int mq1;

int main()
{
	int i;
	int pid;
	char buffer[20];
	start_scheduler();
	mq1 = new_message_queue();
	create_thread(1000000,&ping);
	pid = create_thread(1000000,&pong);
	create_thread(1000000,&pam);
	for(i=0;i<20;i++)
	{
		//sleep(1);
		printf("producteur\n");
		if(i%4==0)
		{
			sprintf(buffer,"hello %d",i);
			printf("envoi de \"hello %d\"\n",i);
			send_message(mq1,buffer,20);
		}
	}
	//del_message_queue(mq1);
	kill(pid);
	i = wait();
	printf("le %deme est mort\n",i);
	i = wait();
	printf("le %deme est mort\n",i);
	send_message(mq1,"KILL",5);
	i = wait();
	printf("le %deme est mort\n",i);
	return 1;
}

void ping()
{
	int i;
	int a;
	char buffer[20] = {0};
	for(i=0;;i++)
	{
		if ((a = receive_message(mq1,buffer,20))==0)
		{
			break;
		}
		printf("ping: message: \"%s\" de taille %d\n",buffer,a);
		if (buffer[0] == 'K')
		{
			break;
		}
	}
}
void pong()
{
	int i;
	for(i=0;i<5;i++)
	{
		printf("pong\n");
		sleep(2);
	}
}

void pam()
{
	int i;
	for(i=0;i<5;i++)
	{
		sleep(1);
		printf("pam\n");
	}
}

