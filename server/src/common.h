/*******************************************************
 * 
 *
*****************************************************/

#ifndef COMMON_H
#define COMMON_H

#include <pthread.h>
#include <semaphore.h>
#include <stdlib.h>

#define FAIL(x) if(x == -1) {\
	perror(#x);exit(-1);}
	

pthread_mutex_t sensorsMutex;

sem_t checkRules;

/**
 * Where to get data?
 */
char *conIP;
int conPort;

/**
 * Where the client's gonna connect?
 */
char *lisIP;
int lisPort;

#endif /*COMMON*/

