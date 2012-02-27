#include <stdio.h>
#include <stdlib.h>
#include "hw.h"
#include "scheduler.h"
#include "semaphore.h"
#include "m_queue.h"


void consommateur();
void producteur();
int mq1;
int sm1;
char mode;

int main(int nargs, char ** args)
{
	int i;
	if (nargs == 1)
	{
		printf("usage: \"./a.out A\" ou \"./a.out B\"\n");
		return;
	}
	mode = args[1][0];
	start_scheduler();
	create_thread(1000000,&producteur);
	i = wait();
	printf("[MAIN] \nle thread n %d est mort\n",i);
	return 1;
}

void producteur()
{
	int i;
	int a;
	int pid;
	char buffer[40] = {0};
	mq1 = new_message_queue();
	sm1 = new_semaphore(0);
	pid = create_thread(1000000,&consommateur);
	for(i=0;i<5;i++)
	{
		sprintf(buffer,"Packet n %d",i);
		printf("[Producteur]\tEnvoi du nouveau packet n %d\n",i);
		send_message(mq1,buffer,40);
		printf("[Producteur]\tAttente de l'accuse de reception\n");
		sem_take(sm1);
		printf("[Producteur]\tAccuse de reception recu\n");
	}
	if (mode == 'A')
	{
		sprintf(buffer,"K",i);
		printf("[Producteur]\tenvoi d'un message de suicide \n");
		send_message(mq1,buffer,2);
	}
	else
	{
		sprintf(buffer,"M",i);
		kill(pid);
		send_message(mq1,buffer,2);
	}
		i = wait();
		printf("[Producteur]\tLe consommateur est mort \n");
		del_message_queue(mq1);
		del_semaphore(sm1);
}
void consommateur()
{
	int i;
	int a;
	char buffer[40] = {0};
	for(i=0;;i++)
	{
		receive_message(mq1,buffer,40);
		if (buffer[0] == 'K')
		{
			printf("[Consommateur]\tReception d'un message de suicide\n");
			printf("[Consommateur]\tSUICIDE\n");
			break;
		}
		printf("[Consommateur]\tNouveau packet recu: %s\n",buffer);
		printf("[Consommateur]\tTraitement...\n");
		sleep(2);
		printf("\n[Consommateur]\tOn accuse de la reception\n");
		sem_give(sm1);
	}
}
