#include "actionneurs.h"

struct trame {
    /*structure definissant une trame reconnaissable par un actionneur, element par element*/
  char* SYNC; //SYNC = "A55A";
  char* HEADER; // (TX MESSAGE): 6
  char* LENGHT; //LENGHT: B
  char* ORG; //ORG : 05
  char* DATA; //DATA : DB3: 0100 0000, le reste a zero => DATA: 50000000 B1
  //DATA : DB3: 0110 0000, le reste a zero => DATA: 70000000 B0
  char* ID; //ID= FF9F1E05
  char* STATUS; //STATUS : doc
  char* CHECKSUM; //CHECKSUM:least significant byte from addition of all bytes except for sync and checksum
};

/*methode permettant de transformer un int en char, ayant une base maximale de 16*/

void itochar(int toBeTrans, char* buffer, int radix) //max base to transform: 16
{
  int i = 0, n, reste;

  char* reverseBuffer = (char*) gMalloc((sizeof (buffer) + 1));
  n = toBeTrans;
  while (n > 0) {
    reste = n % radix;
    n = n / radix;
    switch (reste) { //si la base est > 10, on utilise la notation hexa
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

  strncpy(buffer, reverseBuffer, i); //on copie le numero ecrit dans la nouvelle base dans le char recu en parametre
  gFree(reverseBuffer);//liberation de la memoire temporaire
}

int oneCharHexToInt(char hex) { //conversion d'un seul caractere representant un no en hexo en int
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

int hexToInt(char* hex) { //conversion d'un nb hexa en int
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

/*calcul de la checkSum -> least significant byte de la somme de tous les octets sauf les octets de SYNC */
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
  //HEADER (TX MESSAGE): 6
  //LENGHT: B
  //ORG : 05
  //DATA : DATA: 50000000 B0
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
  //HEADER (TX MESSAGE): 6
  //LENGHT: B
  //ORG : 05
  //DATA : 70000000 B1
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

  strcpy(trameToSend, trameAEnvoyer->SYNC);
  strcat(trameToSend, trameAEnvoyer->HEADER);
  strcat(trameToSend, trameAEnvoyer->LENGHT);
  strcat(trameToSend, trameAEnvoyer->ORG);
  strcat(trameToSend, trameAEnvoyer->DATA);
  strcat(trameToSend, trameAEnvoyer->ID);
  strcat(trameToSend, trameAEnvoyer->STATUS);
  strcat(trameToSend, trameAEnvoyer->CHECKSUM);

  gFree(trameAEnvoyer);

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

/*methode qui reconait l'actionneur qui doit recevoir une trame et qui fait pointer le pointer de fontion sur la bonne methode*/
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
        else if (strcmp(fctName, "previous") == 0) {
          a->fct = pressKEYbackwards;
        }
        else if (strcmp(fctName, "f1") == 0) {
          a->fct = pressKEYF1;
        }
        else if (strcmp(fctName, "f2") == 0) {
          a->fct = pressKEYF2;
        }
        else if (strcmp(fctName, "f3") == 0) {
          a->fct = pressKEYF3;
        }
        else if (strcmp(fctName, "f4") == 0) {
          a->fct = pressKEYF4;
        }
        else if (strcmp(fctName, "f5") == 0) {
          a->fct = pressKEYF5;
        }
        else if (strcmp(fctName, "enter") == 0) {
          a->fct = pressKEYEnter;
        }
        else if (strcmp(fctName, "up") == 0) {
          a->fct = pressKEYUp;
        }
        else if (strcmp(fctName, "down") == 0) {
          a->fct = pressKEYDown;
        }
        else if (strcmp(fctName, "escape") == 0) {
          a->fct = pressKEYEscape;
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
        }else if (strcmp(fctName, "jeu") == 0) {
          a->fct = jeuMYSTERE;
        }else if (strcmp(fctName, "finJeu") == 0) {
          a->fct = finJeuMYSTERE;
        }else {
          printf("Fonction not found : %s\n", fctName);
        }
        break;
      }

    default:
      puts("Actionneur: Type not found");
  }


}
/*methode qui ferme le circuit commande par l'actionneur prise*/
void openCOURRANT(char id[SIZE_ID]) {
  char* trame = (char*) gMalloc(sizeof (char[28]));
  memset(trame, '\0', 28);
  createMessageOpen(id, trame);
  sensorsNetworkSend(trame, 28);
  gFree(trame);
}

/*methode qui ouvre le circuit commande par l'actionneur prise*/
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

/*methode qui simule la frappe d'une touche clavier*/
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
void pressKEYUp(char id[SIZE_ID])
{
  Display *disp = XOpenDisplay (NULL);  
  SendKey (disp, XK_Up , 0);  
}

void pressKEYDown(char id[SIZE_ID])
{
  Display *disp = XOpenDisplay (NULL);  
  SendKey (disp, XK_Down , 0);  
}

void pressKEYforward(char id[SIZE_ID])
{
  Display *disp = XOpenDisplay (NULL);  
  SendKey (disp, XK_Right , 0);  
}

void pressKEYbackwards(char id[SIZE_ID])
{
  Display *disp = XOpenDisplay (NULL);  
  SendKey (disp, XK_Left , 0);  
}

void pressKEYF1(char id[SIZE_ID])
{
  Display *disp = XOpenDisplay (NULL);  
  SendKey (disp, XK_F1 , 0);  
}

void pressKEYF2(char id[SIZE_ID])
{
  Display *disp = XOpenDisplay (NULL);  
  SendKey (disp, XK_F2 , 0);  
}

void pressKEYF3(char id[SIZE_ID])
{
  Display *disp = XOpenDisplay (NULL);  
  SendKey (disp, XK_F3 , 0);  
}

void pressKEYF4(char id[SIZE_ID])
{
  Display *disp = XOpenDisplay (NULL);  
  SendKey (disp, XK_F4 , 0);  
}

void pressKEYF5(char id[SIZE_ID])
{
  Display *disp = XOpenDisplay (NULL);  
  SendKey (disp, XK_F5 , 0);  
}

void pressKEYEnter(char id[SIZE_ID])
{
  Display *disp = XOpenDisplay (NULL);  
  SendKey (disp, XK_Return , 0);  
}

void pressKEYEscape(char id[SIZE_ID])
{
  Display *disp = XOpenDisplay (NULL);  
  SendKey (disp, XK_Escape , 0);  
}

//---- MYSTERE ----------------------------------------------------------------------
FILE *pp; 
FILE *pJ; 
int onGoing = FALSE;
int onGoingJeu = FALSE;

void jeuMYSTERE(char id[SIZE_ID]){
  if(onGoingJeu != TRUE){
    onGoingJeu = TRUE;
    if((pJ = popen("armagetronad", "w")) == NULL){ 
      perror("popen");
      exit(-3);
    }
  }
}

void finJeuMYSTERE(char id[SIZE_ID]){
  if(onGoingJeu == TRUE){
    system("killall -q armagetronad.real");
    pclose(pJ);
    onGoingJeu = FALSE;
  }
}

void actionMYSTERE(char id[SIZE_ID]){
  if(onGoing == TRUE){
    finMYSTERE(NULL);
  }
  onGoing = TRUE;
  if((pp = popen("mplayer weAre.mp3", "w")) == NULL){ 
    perror("popen");
    exit(-3);
  }
}

void finMYSTERE(char id[SIZE_ID]){
  if(onGoing == TRUE){
    fputs("q\n", pp);
    sleep(1);
    pclose(pp);
    onGoing = FALSE;
  }
}

