#include <pthread.h>

#include "../../kernel/memory/memory.h"
#include "guiNetwork.h"
#include "sensorsNetwork.h"
#include "../parse/rules.tab.h"
#include "sensors.h"
#include "initCheckRules.h"
#include "common.h"

#include "init.h"
#include "actionneurs.h"

void init()
{
    initMemory();
    pthread_mutex_init(&sensorsMutex, NULL);

    parseAll();
    initRules();

	printf("Initialisation...\n");
	printf("MemTotale: %ld\n", getGMemTotal());
	printf("MemFree: %ld\n", getGMemFree());

    openCOURRANT("FF9F1E06");

    sensorsNetworkStart();
    guiNetworkStart();
    
/*
    sensors = gMalloc(sizeof(struct sensorType));
    memset(sensors, 0, sizeof(struct sensorType));

    sensors->data = gMalloc(sizeof(dataTEMPERATURE));
    memset(sensors->data, 0, sizeof(dataTEMPERATURE));

    ((dataTEMPERATURE*)sensors->data)->temp = 35;
    strcpy(sensors->id, "cap1");


    sensors->nextSensor = gMalloc(sizeof(struct sensorType));
    memset(sensors->nextSensor, 0, sizeof(struct sensorType));

    sensors->nextSensor->data = gMalloc(sizeof(dataTEMPERATURE));
    memset(sensors->nextSensor->data, 0, sizeof(dataTEMPERATURE));

    ((dataTEMPERATURE*)sensors->nextSensor->data)->temp = 5;
    strcpy(sensors->nextSensor->id, "cap2");
*/
    
}

void destroy()
{
	guiNetworkStop();
    sensorsNetworkStop();
    
	stopRules();
	pthread_mutex_destroy(&sensorsMutex);
	
	destroyMemory();
}
