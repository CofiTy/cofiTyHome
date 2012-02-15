#if ! defined ( ACTIONS_H_ )
#define ACTIONS_H_

#include <stdio.h>
#include <string.h>

#include "common.h"

// ########################### Actions ###########################

struct actionFct_t{
    void (*fct)(char[SIZE_ID]);
    struct actionneur_t * actionneur;
    struct actionFct_t * nextActionFct;
};

struct action_t{
    char nom[SIZE_NAME];
    struct actionFct_t * actionFcts;
    struct action_t * nextAction;
};

struct action_t * actions;

struct action_t * getAction(const char nom[SIZE_NAME]);

void applyAction(struct action_t * action);

void applyActionByName(const char nom[SIZE_NAME]);

void cleanActions();

#endif /*ACTIONS_H_*/
