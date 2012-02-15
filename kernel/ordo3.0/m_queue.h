#ifndef M_QUEUE_H
#define M_QUEUE_H
#include "scheduler.h"
#include "semaphore.h"


int new_message_queue();
void send_message(int mq_id, char* buffer, int size);
int receive_message(int mq_id, char* buffer, int max_size);
int del_message_queue(int mq_id);



#endif
