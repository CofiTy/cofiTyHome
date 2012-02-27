#include "m_queue.h"


/*
 * The structure for chaining messages in the message queue
 */
typedef struct message
{
	char *data;
	int size;
	struct message *next;
}message;

/*
 * The message queue structure
 */
typedef struct message_queue
{
	// message queues are based on semaphore
	// this integer is the id of the semaphore
	// linked to this message queue
	int sem_id;
	// Linked list of messages
	message *messages;
	// The last message (for better adding perf)
	message *last;
}message_queue;

/*
 * Table of active message queues structure
 */
static message_queue *message_queues[256] = {0};


/*
 * Receive a message by a message queue
 * If the message queue is empty, the thread is
 * blocked until a message is sent to this message queue
 * parameters:
 * 	mq_id: the MQ id
 * 	buffer: a pointer to where the message will be written
 * 	max_size: the maximum message size (the size of allocated memory)
 * return value: the length of the message
 */
int receive_message(int mq_id, char* buffer, int max_size)
{
	int i = 0;
	message *to_read;
	// enter in safe mode
	disable_it();
	// if the mq_id is not a valid id
	if ((unsigned)mq_id > 255 || (int)(message_queues[mq_id]) == 0)
	{
		printf("La boite aux lettres numero %d n'existe pas\n",mq_id);
	}
	else
	{
		// Take a coin of the linked semaphore
		sem_take(message_queues[mq_id]->sem_id);
		// copy the message in the buffer
		to_read = message_queues[mq_id]->messages;
		for (i=0;i<to_read->size && i<max_size;i++)
		{
			buffer[i] = to_read->data[i];
		}
		// Unlink the message
		message_queues[mq_id]->messages = message_queues[mq_id]->messages->next;
		if (message_queues[mq_id]->messages == NULL)
		{
			message_queues[mq_id]->last = NULL;
		}
		// release the message
		free(to_read->data);
		free(to_read);
	}
	// leave safe mode
	enable_it();
	return i;
}

/*
 * Receive a message by a message queue
 * If the message queue is empty, the thread is
 * blocked until a message is sent to this message queue
 * parameters:
 * 	mq_id: the MQ id
 * 	buffer: a pointer to where the message will be written
 * 	max_size: the maximum message size (the size of allocated memory)
 * return value: the length of the message
 */
void send_message(int mq_id, char* buffer, int size)
{
	int i;
	message *new;
	// enter in safe mode
	disable_it();
	// if the mq_id is not a valid id
	if ((unsigned)mq_id > 255 || message_queues[mq_id] == 0)
	{
		printf("La boite aux lettres numero %d n'existe pas\n",mq_id);
	}
	else
	{
		// create a new message
		new = malloc(sizeof(message));
		new->data = malloc(size);
		new->size = size;
		new->next = NULL;
		// copy the buffer to the message
		for (i = 0; i < size; i++)
		{
			new->data[i] = buffer[i];
		}
		// link the message
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
		// release a coin to the semaphore
		sem_give(message_queues[mq_id]->sem_id);
	}
	// leave safe mode
	enable_it();
}


/*
 * Delete a message queue
 * A message queue cannot be deleted if a thread is waiting a message from it
 * parameter:
 * 	mq_id: the MQ id
 * return value: -1 if an error occured, 1 otherwise
 */
int del_message_queue(int mq_id)
{
	message *mes;
	// enter in safe mode
	disable_it();
	// if the mq_id is not a valid id
	if ((unsigned)mq_id > 255 || message_queues[mq_id] == 0)
	{
		printf("La boite aux lettres numero %d n'existe pas\n",mq_id);
		// leave safe mode
		enable_it();
		return -1;
	}
	// check if a thread wait for a message
	if (del_semaphore(message_queues[mq_id]->sem_id) == -1)
	{
		printf("Impossible de supprimer la boite aux lettres, des threads sont en attente dessus\n");
		// leave safe mode
		enable_it();
		return -1;
	}
	// a leep for freeing the messages inbox
	while (message_queues[mq_id]->messages != NULL)
	{
		mes = message_queues[mq_id]->messages;
		message_queues[mq_id]->messages = message_queues[mq_id]->messages->next;
		free(mes->data);
		free(mes);
	}
	// free the message queue
	free(message_queues[mq_id]);
	// reset the entry in message queues table
	message_queues[mq_id] = 0;
	// leave safe mode
	enable_it();
	return 1;
}

/*
 * Create a new message queue
 * return value: the MQ id
 */
int new_message_queue()
{
	int id;
	// enter in safe mode
	disable_it();
	// look for a free place in the message queues table
	for(id=0; message_queues[id] != 0;id++);
	// create the structure
	message_queues[id] = (message_queue*)malloc(sizeof(message_queue));
	message_queues[id]->sem_id = new_semaphore(0);
	message_queues[id]->messages = NULL;
	message_queues[id]->last = NULL;
	// leave safe mode
	enable_it();
	return id;
}
