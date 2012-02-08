#if ! defined ( SENSORS_H_ )
#define SENSORS_H_


#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "common.h"

// ########################### Capteurs ##############################

typedef enum typeCapteur {
    TEMPERATURE,
    CONTACT,
    INTERRUPTEUR,
    PRESENCE
} typeCapteur;

typedef struct sensorType {
 typeCapteur type;
 char id[SIZE_ID];
 char name[SIZE_NAME];
 void *data;
 struct sensorType* nextSensor;
 void (*decode)(char* trame, struct sensorType*);
} sensorType;

struct sensorType * sensors;

struct sensorType * getSensor(char id_or_name[MAX(SIZE_ID,SIZE_NAME)]);


//----- TEMPERATURE --------------------------------------------------------------------
typedef struct dataTEMPERATURE {
    int temp;
} dataTEMPERATURE;

void decodeTemperature(char* trame, struct sensorType* capteur);

//----- CONTACT ------------------------------------------------------------------------
typedef struct dataCONTACT {
    int contact;
} dataCONTACT;

void decodeContact(char* trame, struct sensorType* capteur);

//----- PRESENCE ------------------------------------------------------------------------
typedef struct dataPRESENCE {
    int presence;
    int luminosite;
} dataPRESENCE;


void decodePresence(char* trame, struct sensorType* capteur);


//----- INTERRUPTEUR --------------------------------------------------------------------

typedef enum buttonPressed {
    A0,
    A1,
    B0,
    B1,
    NONE
} buttonPressed;

typedef struct dataINTERRUPTEUR {
    buttonPressed switchButton;
} dataINTERRUPTEUR;


void decodeInterrupteur(char* trame, struct sensorType* capteur);

void decodeTrame(char* trame);

#endif /*SENSORS_H_*/
