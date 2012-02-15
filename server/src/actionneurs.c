#include "actionneurs.h"

struct trame {
  char* SYNC; //SYNC = "A55A";
  char* HEADER; // (TX MESSAGE): 3
  char* LENGHT; //LENGHT: B
  char* ORG; //ORG : 05
  char* DATA; //DATA : DB3: 0100 0000, le reste a zero => DATA: 40000000 B1
  //DATA : DB3: 0110 0000, le reste a zero => DATA: 60000000 B0
  char* ID; //ID= FF9F1E05
  char* STATUS; //STATUS : doc
  char* CHECKSUM; //CHECKSUM:least significant byte from addition of all bytes except for sync and checksum
};

void itochar(int toBeTrans, char* buffer, int radix) //max base to transform: 16
{
  int i = 0, n, reste;

  char* reverseBuffer = (char*) gMalloc((sizeof (buffer) + 1));
  n = toBeTrans;
  while (n > 0) {
    reste = n % radix;
    n = n / radix;
    switch (reste) {
      case 10:
        {
          reverseBuffer[i++] = 'A';
          break;
        }
      case 11:
        {
          reverseBuffer[i++] = 'B';
          break;
        }
      case 12:
        {
          reverseBuffer[i++] = 'C';
          break;
        }
      case 13:
        {
          reverseBuffer[i++] = 'D';
          break;
        }
      case 14:
        {
          reverseBuffer[i++] = 'E';
          break;
        }
      case 15:
        {
          reverseBuffer[i++] = 'F';
          break;
        }
      default:
        {
          reverseBuffer[i++] = '0' + reste;
          break;
        }

    }

  }
  reverseBuffer[i] = '\0';

  strncpy(buffer, reverseBuffer, i);
  gFree(reverseBuffer);
}

int oneCharHexToInt(char hex) {
  int result;
  switch (hex) {
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

int hexToInt(char* hex) {
  int result = 0, i, pow = 1, j = 0, cont = 0;
  for (i = 1; i >= 0; i--) {

    for (j = 0; j < cont; j++) {
      pow = pow * 16;
    }
    cont++;
    result += pow * oneCharHexToInt(hex[i]);
  }
  return result;
}

void calculateCheckSum(struct trame* trameAEnv) {
  char* checkSum = (char*) gMalloc(sizeof (char[2]));
  char* lSB = (char*) gMalloc(sizeof (char[2]));
  int sum = 0, i;
  memset(lSB, '\0', 2);
  for (i = 0; i < 8; i = i + 2) {
    checkSum[0] = trameAEnv->DATA[i];
    checkSum[1] = trameAEnv->DATA[i + 1];
    sum += hexToInt(checkSum);
  }

  for (i = 0; i < 8; i = i + 2) {
    checkSum[0] = trameAEnv->ID[i];
    checkSum[1] = trameAEnv->ID[i + 1];
    sum += hexToInt(checkSum);
  }
  checkSum[0] = trameAEnv->HEADER[0];
  checkSum[1] = trameAEnv->LENGHT[0];
  sum += hexToInt(checkSum);
  for (i = 0; i < 2; i = i + 2) {
    checkSum[0] = trameAEnv->ORG[i];
    checkSum[1] = trameAEnv->ORG[i + 1];
    sum += hexToInt(checkSum);
  }
  for (i = 0; i < 2; i = i + 2) {
    checkSum[0] = trameAEnv->STATUS[i];
    checkSum[1] = trameAEnv->STATUS[i + 1];
    sum += hexToInt(checkSum);
  }
  itochar(sum, checkSum, 16);
  lSB[0] = checkSum[1];
  lSB[1] = checkSum[0];
  trameAEnv->CHECKSUM = lSB;
  gFree(checkSum);
}

void createMessageOpen(char id[SIZE_ID], char* trameToSend) {
  /*
  //SYNC: A55A
  //HEADER (TX MESSAGE): 3
  //LENGHT: B
  //ORG : 05
  //DATA : DB3: 0100 0000, le reste a zero => DATA: 40000000 B1
  //ID= FF9F1E05
  //STATUS : 0
  //CHECKSUM:least significant byte from addition of all bytes except for sync and checksum*/
  struct trame* trameAEnvoyer = (struct trame*) gMalloc(sizeof (struct trame));
  memset(trameAEnvoyer, '\0', sizeof (struct trame));
  // char* checkSum = (char*)gMalloc(sizeof(char[2]));
  //char* trameToSend;
  trameAEnvoyer->DATA = "50000000";
  trameAEnvoyer->HEADER = "6";
  trameAEnvoyer->ID = id;
  trameAEnvoyer->LENGHT = "B";
  trameAEnvoyer->ORG = "05";
  trameAEnvoyer->STATUS = "10";
  trameAEnvoyer->SYNC = "A55A";
  calculateCheckSum(trameAEnvoyer);
  //trameAEnvoyer->CHECKSUM = "00";
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

void createMessageClose(char id[SIZE_ID], char* trameToSend) {
  /*
  //SYNC: A55A
  //HEADER (TX MESSAGE): 3
  //LENGHT: B
  //ORG : 05
  //DATA : DB3: 0100 0000, le reste a zero => DATA: 40000000 B1
  //ID= FF9F1E05
  //STATUS : 0
  //CHECKSUM:least significant byte from addition of all bytes except for sync and checksum*/
  struct trame* trameAEnvoyer = (struct trame*) gMalloc(sizeof (struct trame));
  // char* checkSum = (char*)gMalloc(sizeof(char[2]));
  //char* trameToSend;
  trameAEnvoyer->DATA = "70000000";
  trameAEnvoyer->HEADER = "6";
  trameAEnvoyer->ID = id;
  trameAEnvoyer->LENGHT = "B";

  trameAEnvoyer->ORG = "05";
  trameAEnvoyer->STATUS = "10";
  trameAEnvoyer->SYNC = "A55A";
  calculateCheckSum(trameAEnvoyer);
  //trameAEnvoyer->CHECKSUM = "00";
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

struct actionneur_t * getActionneur(char id_or_name[MAX(SIZE_NAME, SIZE_ID)]) {
  struct actionneur_t * current = actionneurs;

  while (current != 0) {
    if (strcmp(current->id, id_or_name) == 0 || (current->name != NULL && strcmp(current->name, id_or_name) == 0)) {
      return current;
    }

    current = current->nextActionneur;
  }

  printf("Actionneur not recognized : %s !!!\n", id_or_name);
  return 0;
}

void setActionneurFct(struct actionFct_t * a, char fctName[SIZE_NAME]) {
  switch (a->actionneur->type) {
    case COURRANT:
      {

        if (strcmp(fctName, "open") == 0) {
          a->fct = openCOURRANT;
        }
        else if (strcmp(fctName, "close") == 0) {
          a->fct = closeCOURRANT;
        }
        else {
          printf("Fonction not found : %s\n", fctName);
        }
        break;
      }
    case VOLETS:
      {

        if (strcmp(fctName, "open") == 0) {
          a->fct = openVOLETS;
        }
        else if (strcmp(fctName, "close") == 0) {
          a->fct = closeVOLETS;
        }
        else {
          printf("Fonction not found : %s\n", fctName);
        }
        break;
      }
    case CAFFE:
      {

        if (strcmp(fctName, "open") == 0) {
          a->fct = openCAFFE;
        }
        else if (strcmp(fctName, "close") == 0) {
          a->fct = closeCAFFE;
        }
        else {
          printf("Fonction not found : %s\n", fctName);
        }
        break;
      }
    case CHAUFFAGE:
      {

        if (strcmp(fctName, "open") == 0) {
          a->fct = openCHAUFFAGE;
        }
        else if (strcmp(fctName, "close") == 0) {
          a->fct = closeCHAUFFAGE;
        }
        else {
          printf("Fonction not found : %s\n", fctName);
        }
        break;
      }

    case VIRTUALKEYBOARD:
      {
        if (strcmp(fctName, "next") == 0) {
          a->fct = pressKEYforward;
        }
        else if (strcmp(fctName, "previous") == 0) {
          a->fct = pressKEYbackwards;
        }
        else {
          printf("Fonction not found : %s\n", fctName);
        }
        break;
      }
    
    case MYSTERE:
      {
        if (strcmp(fctName, "action") == 0) {
          a->fct = actionMYSTERE;
        }else if (strcmp(fctName, "fin") == 0) {
          a->fct = finMYSTERE;
        }else {
          printf("Fonction not found : %s\n", fctName);
        }
        break;
      }

    default:
      puts("Actionneur: Type not found");
  }


}

void openCOURRANT(char id[SIZE_ID]) {
  char* trame = (char*) gMalloc(sizeof (char[28]));
  memset(trame, '\0', 28);
  createMessageOpen(id, trame);
  //puts(trame);
  sensorsNetworkSend(trame, 28);
  gFree(trame);
}

void closeCOURRANT(char id[SIZE_ID]) {
  char* trame = (char*) gMalloc(sizeof (char[28]));
  memset(trame, '\0', 28);
  createMessageClose(id, trame);
  //puts(trame);
  sensorsNetworkSend(trame, 28);
  gFree(trame);
}

void openCAFFE(char id[SIZE_ID]) {
  puts("Demarrer la machine a caffe!!");
}

void closeCAFFE(char id[SIZE_ID]) {
  puts("Arreter la machine a caffe!!");
}

void closeCHAUFFAGE(char id[SIZE_ID]) {
  puts("Arreter le chauffage!");
}

void openCHAUFFAGE(char id[SIZE_ID]) {
  puts("Demarrer le chauffage!");
}

void openVOLETS(char id[SIZE_ID]) {
  puts("Ouvrir les volets");
}

void closeVOLETS(char id[SIZE_ID]) {
  puts("Fermer les volets");
}

void cleanActionneurs(){
  struct actionneur_t * cActionneur;

  while(actionneurs != NULL){
    cActionneur = actionneurs;
    actionneurs = actionneurs->nextActionneur;
    gFree(cActionneur);
  }

}


static void SendKey (Display * disp, KeySym keysym, KeySym modsym)  
{  
  KeyCode keycode = 0, modcode = 0;  

  keycode = XKeysymToKeycode (disp, keysym);  
  if (keycode == 0) return;  

  XTestGrabControl (disp, True);  

  /* Generate modkey press */  
  if (modsym != 0)  
  {  
    modcode = XKeysymToKeycode(disp, modsym);  
    XTestFakeKeyEvent (disp, modcode, True, 0);  
  }  

  /* Generate regular key press and release */  
  XTestFakeKeyEvent (disp, keycode, True, 0);  
  XTestFakeKeyEvent (disp, keycode, False, 0);  

  /* Generate modkey release */  
  if (modsym != 0)  
    XTestFakeKeyEvent (disp, modcode, False, 0);  

  XSync (disp, False);  
  XTestGrabControl (disp, False);  
}

//---- VIRTUALKEYBOARD -------------------------------------
void pressKEYforward(char id[SIZE_ID])
{
  Display *disp = XOpenDisplay (NULL);  
  //sleep (5);  
  SendKey (disp, XK_Right , 0);  
}

void pressKEYbackwards(char id[SIZE_ID])
{
  Display *disp = XOpenDisplay (NULL);  
  //sleep (5);  
  SendKey (disp, XK_Left , 0);  
}

//---- MYSTERE ----------------------------------------------------------------------
FILE *pp; 
int onGoing = FALSE;
void * mplayer(){
  return NULL;
}

void actionMYSTERE(char id[SIZE_ID]){
  if(onGoing == TRUE){
    finMYSTERE(NULL);
  }
  onGoing = TRUE;
  if((pp = popen("mplayer weAre.mp3", "w")) == NULL){ 
      perror("popen"); 
      exit(1);
  }
}

void finMYSTERE(char id[SIZE_ID]){
  fputs("q\n", pp);
  pclose(pp);
  onGoing = FALSE;
}

