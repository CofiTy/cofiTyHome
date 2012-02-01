#if ! defined ( RULES_H_ )
#define RULES_H_

#include <stdio.h>

#include "actions.h"
#include "sensors.h"
// ########################### Règles ################################

//Premier element liste regles
struct rule_t * startRule;

//Une condition
typedef struct condition_t {
	int (*conditionOK)(int *, int); //Fonction a appeler avec leux deux paramètres du dessous. Retourne vrai si la condition est verifiée
	int * data; //Pointeur vers la donnée du capteur a tester
	int value; //Valeur a tester
	struct condition_t * nextCondition; //Pointeur vers la condition suivante
} condition_t;

//Une règle
typedef struct rule_t{
	struct condition_t * conditions;
	struct action_t * action;
	struct rule_t * nextRule;
} rule_t;


void applyRules();

int testEqual(int * data, int value);
int testGreaterOrEqual(int * data, int value);
int testLessOrEqual(int * data, int value);
int testGreater(int * data, int value);
int testLess(int * data, int value);

void setConditionName(struct condition_t * c, char sensorId[9], char cndName[20]);


#endif /*RULES_H_*/



