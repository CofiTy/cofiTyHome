/*******************************************************
 * 
 *
*****************************************************/

#ifndef COMMON_H
#define COMMON_H

#define FAIL(x) if(x == -1) {\
	perror(#x);exit(-1);}
	

pthread_mutex_t sensorsMutex;

sem_t checkRules;


#endif /*COMMON*/

