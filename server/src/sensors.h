
// ########################### Capteurs ##############################
enum buttonPressed {
    A0,
    A1,
    B0,
    B1,
    NOONE
} button;

enum typeCapteur {
    TEMPERATURE,
    CONTACT,
    INTERRUPTEUR,
    PRESENCE
} typeCapteur;

//----- TEMPERATURE
typedef struct dataTEMPERATURE {
    int temp;
} dataTEMPERATURE;

//----- CONTACT
typedef struct dataCONTACT {
    char contact;
} dataCONTACT;

//----- PRESENCE
typedef struct dataPRESENCE {
    char presence1;
    char presence2;
} dataPRESENCE;


//----- TEMPERATURE
typedef struct  {
    int temp;
    char presence;
    char contact;
    enum buttonPressed switchButton;
} output;



 struct sensorType {
 typeCapteur type;
 char id[8];
 char *data;
 struct sensorType* nextSensor;
 void (*decode)(char*, struct sensorType*);
};

typedef struct sensorType sensorList;
