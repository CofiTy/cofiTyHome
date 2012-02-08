/*******************************************************
 * 
 *
*****************************************************/

#ifndef COMMON_H
#define COMMON_H

#include <pthread.h>
#include <semaphore.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define LOG_EXT ".log"

#define FAIL(x) if(x == -1) {\
	perror(#x);exit(-1);}
	

/**
 * Get the semaphore when checking sensors.
 */
pthread_mutex_t sensorsMutex;

/**
 * Get the semaphore when checking Rules.
 */
sem_t checkRules;

/**
 * Where to get data?
 */
char *conIP;
int conPort;

/**
 * Where the client's gonna connect?
 */
int lisPort;

/**
 * Where to log the used rules?
 */
char *nameLogRules;

/**
 * Where to log the sensors data?
 */
char *nameLogSensors;


#endif /*COMMON*/

