
//------------------ACTIONNEUR--------------

#if ! defined ( ACTIONNEURS_H_ )
#define ACTIONNEURS_H_

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <X11/Xlib.h>  
#include <X11/Intrinsic.h>  
#include <X11/extensions/XTest.h>  

#include "actions.h"
#include "sensorsNetwork.h"
#include "../../kernel/memory/memory.h"


// ########################### Actionneurs ###########################

typedef enum typeActionneur 
{
    COURRANT,   
    VOLETS,
    CAFFE,
    CHAUFFAGE,
    VIRTUALKEYBOARD,
    MYSTERE
        
} typeActionneur;

typedef struct actionneur_t {
 typeActionneur type;
 char id[SIZE_ID];
 char name[SIZE_NAME];
 struct actionneur_t* nextActionneur;
} actionneur_t;

struct actionneur_t * actionneurs;

struct actionneur_t * getActionneur(char id_or_name[MAX(SIZE_ID, SIZE_NAME)]);

void setActionneurFct(struct actionFct_t * a, char fctName[SIZE_NAME]);


//----- COURRANT --------------------------------------------------------------------
void openCOURRANT(char id[SIZE_ID]);
void closeCOURRANT(char id[SIZE_ID]);

//----- CAFFE --------------------------------------------------------------------
void openCAFFE(char id[SIZE_ID]);
void closeCAFFE(char id[SIZE_ID]);

//----- VOLETS --------------------------------------------------------------------
void openVOLETS(char id[SIZE_ID]);
void closeVOLETS(char id[SIZE_ID]);

//----- CHAUFFAGE --------------------------------------------------------------------
void openCHAUFFAGE(char id[SIZE_ID]);
void closeCHAUFFAGE(char id[SIZE_ID]);

//----- VIRTUALKEYBOARD -------------------------------------------------------------
void pressKEYforward(char id[SIZE_ID]);
void pressKEYbackwards(char id[SIZE_ID]);
void pressKEYF1(char id[SIZE_ID]);
void pressKEYF2(char id[SIZE_ID]);
void pressKEYF3(char id[SIZE_ID]);
void pressKEYF4(char id[SIZE_ID]);
void pressKEYF5(char id[SIZE_ID]);
void pressKEYFEnter(char id[SIZE_ID]);

//---- MYSTERE ----------------------------------------------------------------------
void actionMYSTERE(char id[SIZE_ID]);
void finMYSTERE(char id[SIZE_ID]);

  void cleanActionneurs();

#endif /*ACTIONNEURS_H_*/

