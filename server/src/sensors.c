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

void decodePresence(char* trame, sensorList* capteur)
{char presence;
    if(trame[15]&'1'=='1')
         presence ='1';
    else
        presence = '0';
    char data[2];
    data[0] = trame[10];
    data[1] = trame[11];
    char* valHex;
    int valDec = strtol(data, &valHex, 16);
    float lumin = ((float)(510.0/255.0))*(float)valDec;
    ((dataPRESENCE*)capteur->data)->presence = presence;
    ((dataPRESENCE*)capteur->data)->luminosite = lumin;
}

void decodeSwitch(char* trame, sensorList* capteur)
{
    char db3 = trame[8];
    switch (db3)
    {
        case '7':
        {
           ((dataINTERRUPTEUR*)capteur->data)->switchButton = B0;
            break;
        }
        case '5':
        {
           ((dataINTERRUPTEUR*)capteur->data)->switchButton  = B1;
            break;
        }
        case '3':
        {
           ((dataINTERRUPTEUR*)capteur->data)->switchButton  = A0;
            break;
        }
        case '1':
        {
          ((dataINTERRUPTEUR*)capteur->data)->switchButton  = A1;
            break;
        }
        default:
        {
            ((dataINTERRUPTEUR*)capteur->data)->switchButton  = NONE;
            break;
            
        }
    }       
}

void decodeContact(char* trame, sensorList* capteur)
{
    char and = '1';
    if(trame[15]&and==and)
    {
        ((dataCONTACT*)capteur->data)->contact = and;
    }
    else
       ((dataCONTACT*)capteur->data)->contact = '0';
}

void decodeTemperature(char* trame, sensorList* capteur)
{
    char data[2];
    data[0] = trame[12];
    data[1] = trame[13];
    char* valHex;
    int valDec = strtol(data, &valHex, 16);
    float temp = ((float)(40.0/255.0))*(float)valDec;
    ((dataTEMPERATURE*)capteur->data)->temp = temp;
}