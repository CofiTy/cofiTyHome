#include "actionneurs.h"
#include <stdlib.h>
#include <math.h>

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
    char* reverseBuffer=(char*)gMalloc((sizeof(buffer)+1));
    n = toBeTrans;
    while(n>0)
    {
        reste = n%radix;
        n = n/radix;
        reverseBuffer[i++]='0'+reste;
        
    }
    reverseBuffer[i]='\0';

    for(j=0;j<i;j++)
    {
        buffer[j] = reverseBuffer[j];
        k++;
    }

    buffer[j] = reverseBuffer[j];
    gFree(reverseBuffer);
}
int oneCharHexToInt(char hex)
{
    int result;
    switch (hex)
    {
            case 'A':
            {
                result = 10;
                break;
            }
            case 'B':
            {
                result = 11;
                break;
            }
            case 'C':
            {
                result = 12;
                break;
            }
            case 'D':
            {
                result = 13;
                break;
            }
            case 'E':
            {
                result = 14;
                break;
            }
            case 'F':
            {
                result = 15;
                break;
            }
            case '0':
            {
                result = 0;
                break;
            }
            case '1':
            {
                result = 1;
                break;
            }
            case '2':
            {
                result = 2;
                break;
            }
            case '3':
            {
                result = 3;
                break;
            }
            case '4':
            {
                result = 4;
                break;
            }
            case '5':
            {
                result = 5;
                break;
            }
            case '6':
            {
                result = 6;
                break;
            }
            case '7':
            {
                result = 7;
                break;
            }
            case '8':
            {
                result = 8;
                break;
            }
            case '9':
            {
                result = 9;
                break;
            }
            default:
            {
                
                break;
            }
    };
    printf("hex to int %d \n", result);  
    return result;
}
int hexToInt(char* hex)
{
    int result=0,i,pow=1,j=0, cont=0;
   // printf("zise of hex: %d", (int)sizeof(hex)/2);
    for(i=1;i>=0;i--)
    {
        
        for(j=0;j<cont;j++)
        {
            pow = pow*16;
        printf("cont1 = %d \n", cont);
        printf("pow1 = %d \n", pow);
        }
        cont++;
        result += pow* oneCharHexToInt(hex[i]);
    }
    printf("cont = %d \n", cont);
    printf("pow = %d \n", pow);
    printf("octet = %d \n", result);
    return result;         
}
void calculateCheckSum(struct trame* trameAEnv)
{
    char* checkSum = (char*)gMalloc(sizeof(char[2]));
    char lSB[2];
    int sum =0, i;
    printf("hello \n");
   // printf("zise of data: %d", (int)sizeof(trameAEnv->DATA));
    for(i=0;i<8;i=i+2)
    {
        checkSum[0] = trameAEnv->DATA[i];
        checkSum[1] = trameAEnv->DATA[i+1];
        sum+= hexToInt(checkSum);
    }
    printf("sumtest DATA %d \n", sum);
    //printf("zise of id: %d", (int)sizeof(trameAEnv->ID));
    for(i=0;i<8;i=i+2)
    {
        checkSum[0] = trameAEnv->ID[i];
        checkSum[1] = trameAEnv->ID[i+1];
        sum+= hexToInt(checkSum);
    }
    printf("sumtest ID %d \n", sum);

        checkSum[0] = trameAEnv->HEADER[0];
        checkSum[1] = trameAEnv->LENGHT[0];
        printf("checksum: %s \n", checkSum);
        sum+= hexToInt(checkSum);
    for(i=0;i<2;i=i+2)
    {
        checkSum[0] = trameAEnv->ORG[i];
        checkSum[1] = trameAEnv->ORG[i+1];
        sum+= hexToInt(checkSum);
    }
    for(i=0;i<2;i=i+2)
    {
        printf("status %s \n", trameAEnv->STATUS);
        checkSum[0] = trameAEnv->STATUS[i];
        checkSum[1] = trameAEnv->STATUS[i+1];
        sum+= hexToInt(checkSum);
    }
/*
    sum+= hexToInt(trameAEnv->HEADER);
    sum+= hexToInt(trameAEnv->ID);
    sum+= hexToInt(trameAEnv->LENGHT);
    sum+= hexToInt(trameAEnv->ORG);
    sum+= hexToInt(trameAEnv->STATUS);
*/
    printf("sum1 %d \n", sum);
    //sum = sum%100;
    
    itochar(sum, checkSum, 16);
    printf("sum2 %s \n", checkSum);  
    lSB[0] = checkSum[1];
    lSB[1] = checkSum[0];
    printf("lsb %s \n", lSB);
    trameAEnv->CHECKSUM = lSB;
    gFree(checkSum);
}

void createMessageOpen(char id[8], char* trameToSend)
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
    struct trame* trameAEnvoyer = (struct trame*)gMalloc(sizeof(struct trame));
   // char* checkSum = (char*)malloc(sizeof(char[2]));
    //char* trameToSend;
    trameAEnvoyer->DATA = "40000000";
    trameAEnvoyer->HEADER = "3";
    trameAEnvoyer->ID =id;
    trameAEnvoyer->LENGHT = "B";
    trameAEnvoyer->STATUS = "00";    
    trameAEnvoyer->ORG = "05";
    trameAEnvoyer->STATUS = "00";
    trameAEnvoyer->SYNC = "A55A";
    calculateCheckSum(trameAEnvoyer);
    //trameAEnvoyer.CHECKSUM = checkSum;
    strcpy(trameToSend, trameAEnvoyer->SYNC);
    strcat(trameToSend, trameAEnvoyer->HEADER);
    strcat(trameToSend, trameAEnvoyer->LENGHT);
    strcat(trameToSend, trameAEnvoyer->ORG);
    strcat(trameToSend, trameAEnvoyer->DATA);
    strcat(trameToSend, trameAEnvoyer->ID);
    strcat(trameToSend, trameAEnvoyer->STATUS);
    strcat(trameToSend, trameAEnvoyer->CHECKSUM);
    printf("trame to send: %c \n", trameToSend);
    printf("id: %c \n", trameAEnvoyer->ID);
    gFree(trameAEnvoyer);
    //return trameToSend;
}

void createMessageClose(char id[9], char* trameToSend)
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
    struct trame* trameAEnvoyer = (struct trame*)gMalloc(sizeof(struct trame));
   // char* checkSum = (char*)malloc(sizeof(char[2]));
    //char* trameToSend;
    trameAEnvoyer->DATA = "60000000";
    trameAEnvoyer->HEADER = "3";
    trameAEnvoyer->ID =id;
    trameAEnvoyer->LENGHT = "B";
       
    trameAEnvoyer->ORG = "05";
    trameAEnvoyer->STATUS = "00";
    trameAEnvoyer->SYNC = "A55A";
    calculateCheckSum(trameAEnvoyer);
    //trameAEnvoyer.CHECKSUM = checkSum;
    strcpy(trameToSend, trameAEnvoyer->SYNC);
    strcat(trameToSend, trameAEnvoyer->HEADER);
    strcat(trameToSend, trameAEnvoyer->LENGHT);
    strcat(trameToSend, trameAEnvoyer->ORG);
    strcat(trameToSend, trameAEnvoyer->DATA);
    strcat(trameToSend, trameAEnvoyer->ID);
    strcat(trameToSend, trameAEnvoyer->STATUS);
    strcat(trameToSend, trameAEnvoyer->CHECKSUM);
    gFree(trameAEnvoyer);
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
    char* trame = (char*)gMalloc(sizeof(char[29]));
    printf("id recu %s \n", id);
    createMessageOpen(id, trame);
    printf("trame a envoyer: %s \n", trame);
    sensorsNetworkSend(trame, sizeof(trame));
    gFree(trame);
}

void closeCOURRANT(char id[9]){
    char* trame = (char*)gMalloc(sizeof(char[29]));
    createMessageClose("FF9F1E05", trame);
    printf("trame a envoyer: %s \n", trame);
    sensorsNetworkSend(trame, sizeof(trame));
    gFree(trame);
}
