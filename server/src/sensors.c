#include "sensors.h"
#include "common.h"
#include "../../kernel/memory/memory.h"

void logValue(char idSensor[SIZE_ID], char nameValue[SIZE_NAME], int value) {
    FILE * pFile;
    pFile = fopen(nameLogSensors, "a+");
    time_t t;
    time(&t);

    if (pFile != NULL) {
        fprintf(pFile, "%d %s %s %d\n", (int)t, idSensor, nameValue, value);
        fclose(pFile);
    } else {
        printf("Le fichier %s n'a pas pu être ouvert !!!\n", nameLogSensors);
    }
}

/*methode permetant de retrouver le capteur a partir de son id ou de son nom
 la liste de capteurs existants est parcurue*/
struct sensorType * getSensor(char id_or_name[MAX(SIZE_NAME,SIZE_ID)]) {
    struct sensorType * current = sensors;

    while (current != NULL) {
        if (strcmp(current->id, id_or_name) == 0 || (current->name != NULL && strcmp(current->name, id_or_name) == 0)) {
            return current;
        }

        current = current->nextSensor;
    }

    return 0;
}

/*methode permettant de decoder une trame*/
void decodeTrame(char* trame) {
    pthread_mutex_lock(&sensorsMutex); //liste de capteurs protegee par mutex

    int i;
    char id[SIZE_ID];
//recuperation de l'ID du capteur emmeteur
    for (i = 0; i < 8; i++) {
        id[i] = trame[i + 16];
    }

    id[8] = '\0';
//recuperation de la structure de donnees correspondante 
    sensorType* capteur = getSensor(id);
    //si le capteur est reconnu, appel a sa methode decode
    if (capteur != NULL) {
        capteur->decode(trame, capteur);
        sem_post(&checkRules); //mise de jeton dans le sem pour le regles -> notification nouvelle trame
        
    }
    pthread_mutex_unlock(&sensorsMutex);//liberation de la liste de capteurs
    
}

/*methode de decodage de donnees du capteur de presence et luminosite*/
void decodePresence(char* trame, struct sensorType* capteur) {
    //TYPE: 06
    //FUNC: 01
    int presence;
    char and = '1';

    if (((trame[15]>>1)&(and)) == and) //le bit 1 du octet 0 , 
        presence = 0;
    else
        presence = 1;

    char data[2];
    data[0] = trame[10];
    data[1] = trame[11];

    char* valHex;

    int valDec = strtol(data, &valHex, 16);

    float lumin = 2*(float) valDec; // 2= ~510/256, conforme a la doc

    ((dataPRESENCE*) capteur->data)->presence = presence;
    ((dataPRESENCE*) capteur->data)->luminosite = lumin;

    logValue(capteur->id, "presence", presence);
    logValue(capteur->id, "luminosite", lumin);

}

/*methode de decodage de donnees de l'interrupteur*/
void decodeInterrupteur(char* trame, struct sensorType* capteur) {
    char db3 = trame[8]; //data bit 3
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
}
/*methode de decodage de donnees du capteur contact fenetre*/
void decodeContact(char* trame, struct sensorType* capteur) {
    char and = '1';
    if (((trame[15])&(and)) == and) {
        ((dataCONTACT*) capteur->data)->contact = 1;
    } else {
        ((dataCONTACT*) capteur->data)->contact = 0;
    }

    logValue(capteur->id, "contact", ((dataCONTACT*) capteur->data)->contact);

}

/*methode de decodage de donnees du capteur de temperature*/
void decodeTemperature(char* trame, struct sensorType* capteur) {
    char data[2];
    data[0] = trame[12];
    data[1] = trame[13];

    char* valHex;
    int valDec = strtol(data, &valHex, 16);
    int temp = 40 - ((float) (40.0 / 255.0)) * valDec; //la formule de conversion lineaire: tempMax-multiplicateur*data

    ((dataTEMPERATURE*) capteur->data)->temp = temp;
    
    logValue(capteur->id, "contact", temp);

}

void decodeHorloge(char* trame, struct sensorType* capteur) {}

void cleanSensors(){
  struct sensorType * cSensor;
  
  while(sensors != NULL){
    cSensor = sensors;
    sensors = sensors->nextSensor;
    gFree(cSensor->data);
    gFree(cSensor);
  }

}
