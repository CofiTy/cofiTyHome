#ifndef SEMAPHORE_H
#define SEMAPHORE_H
#include "scheduler.h"


int new_semaphore(int init);
void sem_give(int sem_id);
void sem_take(int sem_id);
int del_semaphore(int id);

#endif
