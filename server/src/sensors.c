#include "sensors.h"

struct sensorType * getSensor(char id[8]) {
    struct sensorType * current = sensors;
    
    while (current != 0) {
        if (strcmp(current->id, id) == 0) {
            return current;
        }
        
        current = current->nextSensor;
    }
    
    printf("Sensor not recognized : %s !!!\n", id);
    return 0;
}

void decodeTemperature(char* trame, struct sensorType * capteur) {
    ((dataTEMPERATURE*) capteur->data)->temp = B0;
}