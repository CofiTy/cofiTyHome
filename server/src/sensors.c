#include "sensors.h"
#include "common.h"

struct sensorType * getSensor(char id[9]) {
    struct sensorType * current = sensors;
    
    while (current != NULL) {
        if (strcmp(current->id, id) == 0) {
            return current;
        }
        
        current = current->nextSensor;
    }
    
    printf("Sensor not recognized : %s !!!\n", id);
    return 0;
}

void decodeTrame(char* trame)
{
    printf("Searching for corresponding sensor...\n");
    pthread_mutex_lock(&sensorsMutex);
    int i;
    char id[9];
    for(i=0;i<8;i++)
    {
        id[i]=trame[i+16];
    }
    id[8]='\0';
    sensorType* capteur = getSensor(id);
    if(capteur!=NULL)
    {
        capteur->decode(trame,capteur);
        sem_post(&checkRules);
    }
    pthread_mutex_unlock(&sensorsMutex);
    printf("Done searching" );
}
void decodePresence(char* trame, struct sensorType* capteur)
{
    int presence;
    char and = '1';
    if(((trame[15])&(and))==and)
         presence =1;
    else
        presence = 0;
    char data[2];
    data[0] = trame[10];
    data[1] = trame[11];
    char* valHex;
    int valDec = strtol(data, &valHex, 16);
    float lumin = ((float)(510.0/255.0))*(float)valDec;
    ((dataPRESENCE*)capteur->data)->presence = presence;
    ((dataPRESENCE*)capteur->data)->luminosite = lumin;
    printf("decode presence!");
   
}

void decodeInterrupteur(char* trame, struct sensorType* capteur)
{
    char db3 = trame[8];
    switch (db3)
    {
        case '7':
        {
           ((dataINTERRUPTEUR*)capteur->data)->switchButton = B0;
           printf("decode interrupteur!");
            break;
        }
        case '5':
        {
           ((dataINTERRUPTEUR*)capteur->data)->switchButton  = B1;
           printf("decode interrupteur!");
            break;
        }
        case '3':
        {
           ((dataINTERRUPTEUR*)capteur->data)->switchButton  = A0;
           printf("decode interrupteur!");
            break;
        }
        case '1':
        {
          ((dataINTERRUPTEUR*)capteur->data)->switchButton  = A1;
          printf("decode interrupteur!");
            break;
        }
        default:
        {
            ((dataINTERRUPTEUR*)capteur->data)->switchButton  = NONE;
            printf("decode interrupteur!");
            break;
            
        }
    }       
}

void decodeContact(char* trame, struct sensorType* capteur)
{
    char and = '1';
    if(((trame[15])&(and))==and)
    {
        ((dataCONTACT*)capteur->data)->contact = 1;
    }
    else
       ((dataCONTACT*)capteur->data)->contact = 0;
    printf("decode contact fenetre!");
}

void decodeTemperature(char* trame, struct sensorType* capteur)
{
    char data[2];
    data[0] = trame[12];
    data[1] = trame[13];
    char* valHex;
    int valDec = strtol(data, &valHex, 16);
    int temp = ((float)(40.0/255.0))*valDec;
    ((dataTEMPERATURE*)capteur->data)->temp = temp;
    printf("decode Teperature!");
}
