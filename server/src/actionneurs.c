#include "actionneurs.h"

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
    return result;
}
int hexToInt(char* hex)
{
    int result=0,i,pow=1,j=0, cont=0;
    for(i=1;i>=0;i--)
    {
        
        for(j=0;j<cont;j++)
        {
            pow = pow*16;
        }
        cont++;
        result += pow* oneCharHexToInt(hex[i]);
    }
    return result;         
}
void calculateCheckSum(struct trame* trameAEnv)
{
    char* checkSum = (char*)gMalloc(sizeof(char[2]));
    char lSB[2];
    int sum =0, i;
    for(i=0;i<8;i=i+2)
    {
        checkSum[0] = trameAEnv->DATA[i];
        checkSum[1] = trameAEnv->DATA[i+1];
        sum+= hexToInt(checkSum);
    }

    for(i=0;i<8;i=i+2)
    {
        checkSum[0] = trameAEnv->ID[i];
        checkSum[1] = trameAEnv->ID[i+1];
        sum+= hexToInt(checkSum);
    }
    checkSum[0] = trameAEnv->HEADER[0];
    checkSum[1] = trameAEnv->LENGHT[0];
    sum+= hexToInt(checkSum);
    for(i=0;i<2;i=i+2)
    {
        checkSum[0] = trameAEnv->ORG[i];
        checkSum[1] = trameAEnv->ORG[i+1];
        sum+= hexToInt(checkSum);
    }
    for(i=0;i<2;i=i+2)
    {
        checkSum[0] = trameAEnv->STATUS[i];
        checkSum[1] = trameAEnv->STATUS[i+1];
        sum+= hexToInt(checkSum);
    }    
    itochar(sum, checkSum, 16);  
    lSB[0] = checkSum[1];
    lSB[1] = checkSum[0];
    trameAEnv->CHECKSUM = lSB;
    gFree(checkSum);
}

void createMessageOpen(char id[9], char* trameToSend)
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
    trameAEnvoyer->DATA = "50000000";
    trameAEnvoyer->HEADER = "6";
    trameAEnvoyer->ID =id;
    trameAEnvoyer->LENGHT = "B";
    trameAEnvoyer->ORG = "05";
    trameAEnvoyer->STATUS = "30";    
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
    //strcat(trameToSend, '\0');
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
    trameAEnvoyer->DATA = "70000000";
    trameAEnvoyer->HEADER = "6";
    trameAEnvoyer->ID =id;
    trameAEnvoyer->LENGHT = "B";
       
    trameAEnvoyer->ORG = "05";
    trameAEnvoyer->STATUS = "30";
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
    //strcat(trameToSend, '\0');
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
    char* trame = (char*)gMalloc(sizeof(char[28]));
    memset(trame, '\0', 28);
    createMessageOpen(id, trame);
    sensorsNetworkSend(trame, 28);
    gFree(trame);
}

void closeCOURRANT(char id[9]){
    char* trame = (char*)gMalloc(sizeof(char[28]));
    memset(trame, '\0', 28);
    createMessageClose(id, trame);
    sensorsNetworkSend(trame, 28);
    gFree(trame);
}
