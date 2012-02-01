#include "actionneurs.h"
#include <stdlib.h>

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

void CalculateCheckSum(struct trame trameAEnv)
{
    char checkSum[2];
    int sum =0;
    sum+= atoi(trameAEnv.DATA);
    sum+= atoi(trameAEnv.HEADER);
    sum+= atoi(trameAEnv.ID);
    sum+= atoi(trameAEnv.LENGHT);
    sum+= atoi(trameAEnv.ORG);
    sum+= atoi(trameAEnv.STATUS);
    itochar(sum, checkSum, 10);
    trameAEnv.CHECKSUM = checkSum;
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
    struct trame trameAEnvoyer ;//= (trame)malloc(sizeof(trame));
   // char* checkSum = (char*)malloc(sizeof(char[2]));
    //char* trameToSend;
    trameAEnvoyer.DATA = "40000000";
    trameAEnvoyer.HEADER = "3";
    trameAEnvoyer.ID =id;
    trameAEnvoyer.LENGHT = "B";
    trameAEnvoyer.STATUS = "00";    
    trameAEnvoyer.ORG = "05";
    trameAEnvoyer.STATUS = "30";
    trameAEnvoyer.SYNC = "A55A";
    CalculateCheckSum(trameAEnvoyer);
    //trameAEnvoyer.CHECKSUM = checkSum;
    strcpy(trameToSend, trameAEnvoyer.SYNC);
    strcat(trameToSend, trameAEnvoyer.HEADER);
    strcat(trameToSend, trameAEnvoyer.LENGHT);
    strcat(trameToSend, trameAEnvoyer.ORG);
    strcat(trameToSend, trameAEnvoyer.DATA);
    strcat(trameToSend, trameAEnvoyer.ID);
    strcat(trameToSend, trameAEnvoyer.STATUS);
    strcat(trameToSend, trameAEnvoyer.CHECKSUM);
    //return trameToSend;
}

void OpenCourrant(char* id)
{
    char* trame = "";
    CreateMessageOpen("FF9F1E05", trame);
    printf("la trame cree: %s \n", trame);
}