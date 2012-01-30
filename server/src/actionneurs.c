#include "actionneurs.h"

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
    printf("%s : openCOURRANT\n", id);
}

void closeCOURRANT(char id[9]){
    printf("%s : closeCOURRANT\n", id);
}