#ifndef M_QUEUE_H
#define M_QUEUE_H
#include "scheduler.h"
#include "semaphore.h"

/*
 * This library provides message queues
 */

/*
 * Create a new message queue
 * return value: the MQ id
 */
int new_message_queue();

/*
 * Send a message via a message queue
 * parameters:
 * 	mq_id: the MQ id
 * 	buffer: a pointer to the message to send
 * 	size: the size of the message
 */
void send_message(int mq_id, char* buffer, int size);

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
int receive_message(int mq_id, char* buffer, int max_size);

/*
 * Delete a message queue
 * A message queue cannot be deleted if a thread is waiting a message from it
 * parameter:
 * 	mq_id: the MQ id
 * return value: -1 if an error occured, 1 otherwise
 */
int del_message_queue(int mq_id);



#endif
