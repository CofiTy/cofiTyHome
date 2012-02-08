#include "sensors.h"
#include "common.h"

void logValue(char idSensor[9], char nameValue[20], int value) {
    FILE * pFile;
    puts("try Open");
    pFile = fopen(nameLogSensors, "a+");
    puts("Open");
    time_t t;
    time(&t);

    if (pFile != NULL) {
        fprintf(pFile, "%d %s %s %d\n", (int)t, idSensor, nameValue, value);
        puts("Write");
        fclose(pFile);
    } else {
        printf("Le fichier %s n'a pas pu être ouvert !!!\n", nameLogSensors);
    }
}

struct sensorType * getSensor(char id_or_name[20]) {
    struct sensorType * current = sensors;

    while (current != NULL) {
        if (strcmp(current->id, id_or_name) == 0 || (current->name != NULL && strcmp(current->name, id_or_name) == 0)) {
            return current;
        }

        current = current->nextSensor;
    }

    printf("Sensor not recognized : %s !!!\n", id_or_name);
    return 0;
}

void decodeTrame(char* trame) {
    //printf("Searching for corresponding sensor...\n");
    pthread_mutex_lock(&sensorsMutex);

    int i;
    char id[9];

    for (i = 0; i < 8; i++) {
        id[i] = trame[i + 16];
    }

    id[8] = '\0';

    sensorType* capteur = getSensor(id);

    if (capteur != NULL) {
        capteur->decode(trame, capteur);
        sem_post(&checkRules);
        
    }
    pthread_mutex_unlock(&sensorsMutex);
    
}

void decodePresence(char* trame, struct sensorType* capteur) {
    int presence;
    char and = '1';

    if (((trame[15])&(and)) == and)
        presence = 1;
    else
        presence = 0;

    char data[2];
    data[0] = trame[10];
    data[1] = trame[11];

    char* valHex;

    int valDec = strtol(data, &valHex, 16);

    float lumin = (116.0)*(float) valDec+300;

    ((dataPRESENCE*) capteur->data)->presence = presence;
    ((dataPRESENCE*) capteur->data)->luminosite = lumin;

    logValue(capteur->id, "presence", presence);
    logValue(capteur->id, "luminosite", lumin);
    printf("valeur de la luminosite: %d, valeur presence %d \n",((dataPRESENCE*) capteur->data)->luminosite, ((dataPRESENCE*) capteur->data)->presence );
    printf("decode presence!\n");

}

void decodeInterrupteur(char* trame, struct sensorType* capteur) {
    char db3 = trame[8];
    switch (db3) {
        case '7':
        {
            ((dataINTERRUPTEUR*) capteur->data)->switchButton = B0;

            break;
        }
        case '5':
        {
            ((dataINTERRUPTEUR*) capteur->data)->switchButton = B1;

            break;
        }
        case '3':
        {
            ((dataINTERRUPTEUR*) capteur->data)->switchButton = A0;

            break;
        }
        case '1':
        {
            ((dataINTERRUPTEUR*) capteur->data)->switchButton = A1;

            break;
        }
        default:
        {
            ((dataINTERRUPTEUR*) capteur->data)->switchButton = NONE;

            break;

        }
    }
            logValue(capteur->id, "switchButton", ((dataINTERRUPTEUR*) capteur->data)->switchButton);
            printf("decode interrupteur!\n");
}

void decodeContact(char* trame, struct sensorType* capteur) {
    char and = '1';
    if (((trame[15])&(and)) == and) {
        ((dataCONTACT*) capteur->data)->contact = 1;
    } else {
        ((dataCONTACT*) capteur->data)->contact = 0;
    }

    logValue(capteur->id, "contact", ((dataCONTACT*) capteur->data)->contact);

    printf("decode contact fenetre!\n");
}

void decodeTemperature(char* trame, struct sensorType* capteur) {
    char data[2];
    data[0] = trame[12];
    data[1] = trame[13];

    char* valHex;
    int valDec = strtol(data, &valHex, 16);
    int temp = ((float) (40.0 / 255.0)) * valDec;

    ((dataTEMPERATURE*) capteur->data)->temp = temp;
    
    logValue(capteur->id, "contact", temp);

    printf("decode Temperature!\n");
}
