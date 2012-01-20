#include "sensors.h"

sensorType * getSensor(char id[8]) {
    sensorType * current = sensors;

    while (current != 0) {
        if (memcmp(current->id, id, sizeof (char) *8)) {
            return current;
        }

        return 0;
    }
}

void decodeTemperature(char* trame, sensorType * capteur) {
    ((dataTEMPERATURE*) capteur->data)->temp = B0;
}