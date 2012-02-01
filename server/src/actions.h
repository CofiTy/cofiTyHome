#if ! defined ( ACTIONS_H_ )
#define ACTIONS_H_

#include <stdio.h>
#include <string.h>

// ########################### Actions ###########################

struct actionFct_t{
    void (*fct)(char[9]);
    struct actionneur_t * actionneur;
    struct actionFct_t * nextActionFct;
};

struct action_t{
    char nom[20];
    struct actionFct_t * actionFcts;
    struct action_t * nextAction;
};

struct action_t * actions;

struct action_t * getAction(char nom[20]);

void applyAction(struct action_t * action);


#endif /*ACTIONS_H_*/
