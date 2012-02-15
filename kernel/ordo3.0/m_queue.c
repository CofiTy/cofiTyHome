#include "m_queue.h"

typedef struct message
{
	char *data;
	int size;
	struct message *next;
}message;

typedef struct message_queue
{
	int sem_id;
	message *messages;
	message *last;
}message_queue;
static message_queue *message_queues[256] = {0};

int receive_message(int mq_id, char* buffer, int max_size)
{
	int i = 0;
	message *to_read;
	disable_it();
	if ((unsigned)mq_id > 255 || (int)(message_queues[mq_id]) == 0)
	{
		printf("La boite aux lettres numero %d n'existe pas\n",mq_id);
	}
	else
	{
		sem_take(message_queues[mq_id]->sem_id);
		to_read = message_queues[mq_id]->messages;
		for (i=0;i<to_read->size && i<max_size;i++)
		{
			buffer[i] = to_read->data[i];
		}
		message_queues[mq_id]->messages = message_queues[mq_id]->messages->next;
		if (message_queues[mq_id]->messages == NULL)
		{
			message_queues[mq_id]->last = NULL;
		}
		free(to_read->data);
		free(to_read);
	}
	enable_it();
	return i;
}

void send_message(int mq_id, char* buffer, int size)
{
	int i;
	message *new;
	disable_it();
	if ((unsigned)mq_id > 255 || message_queues[mq_id] == 0)
	{
		printf("La boite aux lettres numero %d n'existe pas\n",mq_id);
	}
	else
	{
		new = malloc(sizeof(message));
		new->data = malloc(size);
		new->size = size;
		new->next = NULL;
		for (i = 0; i < size; i++)
		{
			new->data[i] = buffer[i];
		}
		if (message_queues[mq_id]->last == NULL)
		{
			message_queues[mq_id]->last = new;
			message_queues[mq_id]->messages = new;
		}
		else
		{
			message_queues[mq_id]->last->next = new;
			message_queues[mq_id]->last = new;
		}
		sem_give(message_queues[mq_id]->sem_id);
	}
	enable_it();
}


int del_message_queue(int mq_id)
{
	message *mes;
	disable_it();
	if ((unsigned)mq_id > 255 || message_queues[mq_id] == 0)
	{
		printf("La boite aux lettres numero %d n'existe pas\n",mq_id);
		enable_it();
		return -1;
	}
	if (del_semaphore(message_queues[mq_id]->sem_id) == -1)
	{
		printf("Impossible de supprimer la boite aux lettres, des threads sont en attente dessus\n");
		enable_it();
		return -1;
	}
	while (message_queues[mq_id]->messages != NULL)
	{
		mes = message_queues[mq_id]->messages;
		message_queues[mq_id]->messages = message_queues[mq_id]->messages->next;
		free(mes->data);
		free(mes);
	}
	free(message_queues[mq_id]);
	message_queues[mq_id] = 0;
	enable_it();
	return 1;
}

int new_message_queue()
{
	int id;
	disable_it();
	for(id=0; message_queues[id] != 0;id++);
	message_queues[id] = (message_queue*)malloc(sizeof(message_queue));
	message_queues[id]->sem_id = new_semaphore(0);
	message_queues[id]->messages = NULL;
	message_queues[id]->last = NULL;
	enable_it();
	return id;
}
