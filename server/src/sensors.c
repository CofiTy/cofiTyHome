#include "sensors.h"

struct sensorType * getSensor(char id[9]) {
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


void decodeContact(char* trame, struct sensorType* capteur){

}

void decodePresence(char* trame, struct sensorType* capteur){

}

void decodeInterrupteur(char* trame, struct sensorType* capteur){

}