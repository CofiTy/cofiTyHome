#include "stdio.h"
#include "string.h"

// ########################### Capteurs ##############################

typedef enum typeCapteur {
    TEMPERATURE,
    CONTACT,
    INTERRUPTEUR,
    PRESENCE
} typeCapteur;

typedef struct sensorType {
 typeCapteur type;
 char id[8];
 void *data;
 struct sensorType* nextSensor;
 void (*decode)(char* trame, struct sensorType*);
} sensorType;

sensorType * sensors;

sensorType * getSensor(char id[8]);

//sensorType sensorList;


//----- TEMPERATURE --------------------------------------------------------------------
typedef struct dataTEMPERATURE {
    int temp;
} dataTEMPERATURE;

void decodeTemperature(char* trame, sensorType* capteur);

//----- CONTACT ------------------------------------------------------------------------
typedef struct dataCONTACT {
    char contact;
} dataCONTACT;

void decodeContact(char* trame, sensorType* capteur);

//----- PRESENCE ------------------------------------------------------------------------
typedef struct dataPRESENCE {
    char presence;
    char luminosite;
} dataPRESENCE;

#
void decodePresence(char* trame, sensorType* capteur);


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


void decodeInterrupteur(char* trame, sensorType* capteur);
