#include "rules.h"

void applyRules(){
    struct rule_t * currentRule = startRule;
    struct condition_t * currentCondition;
    int b = 1;

    while(currentRule != 0){
        currentCondition = currentRule->conditions;
        b = 1;

        while(currentCondition != 0){
            if(currentCondition->conditionOK(currentCondition->data, currentCondition->value) == 0){
                b = 0;
                break;
            }

            currentCondition = currentCondition->nextCondition;
        }

        if(b == 1){

            printf("TOUT OK!!!\n");
            //TODO:Actions
            //currentRule->action;
        } else {
            printf("FAIL\n");
        }

        currentRule = currentRule->nextRule;
    }
}

int testEqual(int * data, int value) {
    if(*data == value)
        return 1;
    else
        return 0;
}

int testGreaterOrEqual(int * data, int value) {
    if(*data >= value)
        return 1;
    else
        return 0;
}

int testLessOrEqual(int * data, int value) {
    if(*data <= value)
        return 1;
    else
        return 0;
}

int testGreater(int * data, int value) {
    if(*data > value){
        return 1;
    } else {
        return 0;
    }
}

int testLess(int * data, int value) {
    if(*data < value)
        return 1;
    else
        return 0;
}
