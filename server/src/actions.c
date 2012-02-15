#include "actions.h"
#include "actionneurs.h"

struct action_t * getAction(const char nom[SIZE_NAME]) {
  struct action_t * current = actions;

  while (current != 0) {
    if (strcmp(current->nom, nom) == 0) {
      return current;
    }

    current = current->nextAction;
  }

  printf("Action not recognized : %s !!!\n", nom);
  return 0;
}

void applyAction(struct action_t * action){
  struct actionFct_t * current = action->actionFcts;

  while (current != 0) {
    current->fct(current->actionneur->id);

    current = current->nextActionFct;
  }
}

void applyActionByName(const char nom[SIZE_NAME]){
  applyAction(getAction(nom));
}

void cleanActions(){
  struct action_t * cAction;
  struct actionFct_t * cActionFct;

  while(actions != NULL){
    while(actions->actionFcts != NULL){
      cActionFct = actions->actionFcts;
      actions->actionFcts = actions->actionFcts->nextActionFct;
      gFree(cActionFct);
    }
    cAction = actions;
    actions = actions->nextAction;
    gFree(cAction);
  }

}
