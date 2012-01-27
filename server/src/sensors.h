#if ! defined ( SENSORS_H_ )
#define SENSORS_H_


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

struct sensorType * sensors;

struct sensorType * getSensor(char id[8]);

//sensorType sensorList;


//----- TEMPERATURE --------------------------------------------------------------------
typedef struct dataTEMPERATURE {
    int temp;
} dataTEMPERATURE;

void decodeTemperature(char* trame, struct sensorType* capteur);

//----- CONTACT ------------------------------------------------------------------------
typedef struct dataCONTACT {
    char contact;
} dataCONTACT;

void decodeContact(char* trame, struct sensorType* capteur);

//----- PRESENCE ------------------------------------------------------------------------
typedef struct dataPRESENCE {
    char presence;
    char luminosite;
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

#endif /*SENSORS_H_*/
