
//------------------ACTIONNEUR--------------

#if ! defined ( ACTIONNEURS_H_ )
#define ACTIONNEURS_H_

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "actions.h"
#include "sensorsNetwork.h"
#include "../../kernel/memory/memory.h"


// ########################### Actionneurs ###########################

typedef enum typeActionneur {
    COURRANT
} typeActionneur;

typedef struct actionneur_t {
 typeActionneur type;
 char id[9];
 struct actionneur_t* nextActionneur;
} actionneur_t;

struct actionneur_t * actionneurs;

struct actionneur_t * getActionneur(char id[9]);

void setActionneurFct(struct actionFct_t * a, char fctName[20]);


//----- COURRANT --------------------------------------------------------------------
void openCOURRANT(char id[9]);
void closeCOURRANT(char id[9]);

#endif /*ACTIONNEURS_H_*/

