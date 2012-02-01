#include "actionneurs.h"
#include <stdlib.h>

struct trame{ 
        char* SYNC;//SYNC = "A55A";
        char* HEADER;// (TX MESSAGE): 3
        char* LENGHT;//LENGHT: B
        char* ORG;//ORG : 05
        char* DATA;//DATA : DB3: 0100 0000, le reste a zero => DATA: 40000000 B1
//DATA : DB3: 0110 0000, le reste a zero => DATA: 60000000 B0
        char* ID;//ID= FF9F1E05
        char* STATUS;//STATUS : doc
        char* CHECKSUM;//CHECKSUM:least significant byte from addition of all bytes except for sync and checksum
};

void itochar(int toBeTrans, char* buffer, int radix )
{
    int i=0, n, reste,j,k=0;
    char* reverseBuffer=(char*)malloc((sizeof(buffer)+1));
    n = toBeTrans;
    while(n>0)
    {
        reste = n%radix;
        n = n/radix;
        reverseBuffer[i++]='0'+reste;
        
    }
    reverseBuffer[i]='\0';
    for(j=i-1;j>=0;j--)
    {
        buffer[k] = reverseBuffer[j];
        k++;
    }
    buffer[k]='\0';
    free(reverseBuffer);
}

void CalculateCheckSum(struct trame* trameAEnv)
{
    char* checkSum = (char*)malloc(sizeof(char[2]));
    int sum =0;
    sum+= atoi(trameAEnv->DATA);
    sum+= atoi(trameAEnv->HEADER);
    sum+= atoi(trameAEnv->ID);
    sum+= atoi(trameAEnv->LENGHT);
    sum+= atoi(trameAEnv->ORG);
    sum+= atoi(trameAEnv->STATUS);
    sum = sum%100;
    itochar(sum, checkSum, 10);
    trameAEnv->CHECKSUM = checkSum;
}

void CreateMessageOpen(char id[8], char* trameToSend)
{
    /*
//SYNC: A55A
//HEADER (TX MESSAGE): 3
//LENGHT: B
//ORG : 05
//DATA : DB3: 0100 0000, le reste a zero => DATA: 40000000 B1
//ID= FF9F1E05
//STATUS : 0
//CHECKSUM:least significant byte from addition of all bytes except for sync and checksum*/
    struct trame* trameAEnvoyer = (struct trame*)malloc(sizeof(struct trame));
   // char* checkSum = (char*)malloc(sizeof(char[2]));
    //char* trameToSend;
    trameAEnvoyer->DATA = "40000000";
    trameAEnvoyer->HEADER = "3";
    trameAEnvoyer->ID =id;
    trameAEnvoyer->LENGHT = "B";
    trameAEnvoyer->STATUS = "00";    
    trameAEnvoyer->ORG = "05";
    trameAEnvoyer->STATUS = "30";
    trameAEnvoyer->SYNC = "A55A";
    CalculateCheckSum(trameAEnvoyer);
    //trameAEnvoyer.CHECKSUM = checkSum;
    strcpy(trameToSend, trameAEnvoyer->SYNC);
    strcat(trameToSend, trameAEnvoyer->HEADER);
    strcat(trameToSend, trameAEnvoyer->LENGHT);
    strcat(trameToSend, trameAEnvoyer->ORG);
    strcat(trameToSend, trameAEnvoyer->DATA);
    strcat(trameToSend, trameAEnvoyer->ID);
    strcat(trameToSend, trameAEnvoyer->STATUS);
    strcat(trameToSend, trameAEnvoyer->CHECKSUM);
    //return trameToSend;
}

void CreateMessageClose(char id[9], char* trameToSend)
{
    /*
//SYNC: A55A
//HEADER (TX MESSAGE): 3
//LENGHT: B
//ORG : 05
//DATA : DB3: 0100 0000, le reste a zero => DATA: 40000000 B1
//ID= FF9F1E05
//STATUS : 0
//CHECKSUM:least significant byte from addition of all bytes except for sync and checksum*/
    struct trame* trameAEnvoyer = (struct trame*)malloc(sizeof(struct trame));
   // char* checkSum = (char*)malloc(sizeof(char[2]));
    //char* trameToSend;
    trameAEnvoyer->DATA = "60000000";
    trameAEnvoyer->HEADER = "3";
    trameAEnvoyer->ID =id;
    trameAEnvoyer->LENGHT = "B";
    trameAEnvoyer->STATUS = "00";    
    trameAEnvoyer->ORG = "05";
    trameAEnvoyer->STATUS = "30";
    trameAEnvoyer->SYNC = "A55A";
    CalculateCheckSum(trameAEnvoyer);
    //trameAEnvoyer.CHECKSUM = checkSum;
    strcpy(trameToSend, trameAEnvoyer->SYNC);
    strcat(trameToSend, trameAEnvoyer->HEADER);
    strcat(trameToSend, trameAEnvoyer->LENGHT);
    strcat(trameToSend, trameAEnvoyer->ORG);
    strcat(trameToSend, trameAEnvoyer->DATA);
    strcat(trameToSend, trameAEnvoyer->ID);
    strcat(trameToSend, trameAEnvoyer->STATUS);
    strcat(trameToSend, trameAEnvoyer->CHECKSUM);
    //return trameToSend;
}

struct actionneur_t * getActionneur(char id[9]) {
    struct actionneur_t * current = actionneurs;

    while (current != 0) {
        if (strcmp(current->id, id) == 0) {
            return current;
        }

        current = current->nextActionneur;
    }

    printf("Actionneur not recognized : %s !!!\n", id);
    return 0;
}

void setActionneurFct(struct actionFct_t * a, char fctName[20]){
    if(a->actionneur->type == COURRANT){
        if(strcmp(fctName, "open") == 0){
            a->fct = openCOURRANT;
        } else if(strcmp(fctName, "close") == 0){
            a->fct = closeCOURRANT;
        } else {
            printf("Fonction not found : %s\n", fctName);
        }
    } else {
        printf("Actionneur type not found : %s\n", a->actionneur->id);
    }

}

void openCOURRANT(char id[9]){
    char* trame = (char*)malloc(sizeof(char[29]));
    CreateMessageOpen("FF9F1E05", trame);
    printf("la trame cree: %s \n", trame);
    free(trame);
}

void closeCOURRANT(char id[9]){
    char* trame = (char*)malloc(sizeof(char[29]));
    CreateMessageClose("FF9F1E05", trame);
    printf("la trame cree: %s \n", trame);
    free(trame);
}