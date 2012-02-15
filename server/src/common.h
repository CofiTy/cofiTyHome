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

/**
 * Extension that we use for logs.
 */
#define LOG_EXT ".log"

/**
 * Size of name parameter.
 */
#define SIZE_NAME 20

/**
 * Size of ID parameters.
 */
#define SIZE_ID 9

/**
 * USEFUL
 */ 
#define MAX(x,y) ((x>y)?x:y)

/**
 * IDEM
 */
#define FAIL(x) if(x == -1) {\
	perror(#x);exit(-1);}
	
#define ERROR -1

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

