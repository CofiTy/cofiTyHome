#include <stdlib.h>

#include "rules.h"
#include "common.h"

void logRule(char name[SIZE_NAME]) {

    FILE *fLogRule;
    time_t t;
    time(&t);

    fLogRule = fopen(nameLogRules, "a+");
    if (fLogRule != NULL) {
        fprintf(fLogRule, "%d %s\n", (int) t, name);
        fclose(fLogRule);
    } else {
        printf("No File to Log Rules...\n");
    }
}

void applyRules() {

    //printf("\nApplying rules..\n");

    pthread_mutex_lock(&sensorsMutex);
    struct rule_t * currentRule = startRule;
    struct condition_t * currentCondition;
    int b = 1;

    while (currentRule != 0) {
        currentCondition = currentRule->conditions;
        b = 1;

        while (currentCondition != 0) {
            if (currentCondition->conditionOK(currentCondition->data, currentCondition->value) == 0) {
                b = 0;
                break;
            }

            currentCondition = currentCondition->nextCondition;
        }

        if (b == 1) {

            //printf("TOUT OK!!!\n");
            logRule(currentRule->name);
            applyAction(currentRule->action);
        } else {
            //printf("FAIL\n");
        }

        currentRule = currentRule->nextRule;
    }

    pthread_mutex_unlock(&sensorsMutex);
}

int testEqual(int * data, int value) {
    //printf("testEqual %d  :  %d\n", *data, value);
    if (*data == value)
        return 1;
    else
        return 0;
}

int testGreaterOrEqual(int * data, int value) {
    //printf("testGreaterOrEqual %d  :  %d\n", *data, value);
    if (*data >= value)
        return 1;
    else
        return 0;
}

int testLessOrEqual(int * data, int value) {
    //printf("testLessOrEqual %d  :  %d\n", *data, value);
    if (*data <= value)
        return 1;
    else
        return 0;
}

int testGreater(int * data, int value) {
    //printf("testGreater %d  :  %d\n", *data, value);
    if (*data > value) {
        return 1;
    } else {
        return 0;
    }
}

int testLess(int * data, int value) {
    //printf("testLess %d  :  %d\n", *data, value);
    if (*data < value)
        return 1;
    else
        return 0;
}

void setConditionName(struct condition_t * c, char sensorId[SIZE_ID], char cndName[SIZE_NAME]) {
    char t[SIZE_ID];
    memset(t, '\0', sizeof (char) * SIZE_ID);
    strcpy(t, sensorId);

    sensorType * sensor = getSensor(t);

    int ok = 0;

    if (sensor->type == TEMPERATURE) {
        if (strcmp(cndName, "temp") == 0) {
            c->data = &(((dataTEMPERATURE*) sensor->data)->temp);
        } else {
            ok = 1;
        }
    } else if (sensor->type == CONTACT) {
        if (strcmp(cndName, "contact") == 0) {
            c->data = &(((dataCONTACT*) sensor->data)->contact);
        } else {
            ok = 1;
        }
    } else if (sensor->type == INTERRUPTEUR) {
        if (strcmp(cndName, "switchButton") == 0) {
            c->data = ((int*) &(((dataINTERRUPTEUR*) sensor->data)->switchButton));
        } else {
            ok = 1;
        }
    } else if (sensor->type == PRESENCE) {
        if (strcmp(cndName, "presence") == 0) {
            c->data = &(((dataPRESENCE*) sensor->data)->presence);
        } else if (strcmp(cndName, "luminosite") == 0) {
            c->data = &(((dataPRESENCE*) sensor->data)->luminosite);
        } else {
            ok = 1;
        }
    }

    if (ok == 1) {
        printf("La propriété %s du capteur %s n'existe pas !\n", cndName, sensorId);
        exit(EXIT_FAILURE);
    }
}
