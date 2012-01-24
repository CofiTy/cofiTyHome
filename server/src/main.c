#include <unistd.h>

#include "guiNetwork.h"
#include "rules.tab.h"
#include "sensors.h"

int main(int argc, char ** argv){
/*
    guiNetworkStart();
    sleep(120);
    guiNetworkStop();
*/
    
/*
    sensors = calloc(1, sizeof(struct sensorType));

    sensors->data = calloc(1, sizeof(dataTEMPERATURE));

    ((dataTEMPERATURE*)sensors->data)->temp = 35;
    strcpy(sensors->id, "cap1");


    sensors->nextSensor = calloc(1, sizeof(struct sensorType));

    sensors->nextSensor->data = calloc(1, sizeof(dataTEMPERATURE));

    ((dataTEMPERATURE*)sensors->nextSensor->data)->temp = 5;
    strcpy(sensors->nextSensor->id, "cap2");
*/

    
    parseAll();

    applyRules();

  return 0;
}

