#include "actionneurs.h"

struct actionneur_t * getActionneur(char id[8]) {
    struct actionneur_t * current = sensors;

    while (current != 0) {
        if (strcmp(current->id, id) == 0) {
            return current;
        }

        current = current->nextActionneur;
    }

    printf("Actionneur not recognized : %s !!!\n", id);
    return 0;
}

void caca(){
    
}

void openCOURRANT(char id[8]){

}

void closeCOURRANT(char id[8]){
    
}