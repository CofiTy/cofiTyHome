// ########################### Règles ################################

//
void applyRules();

//Premier element liste regles
struct RULE * startRule;

//Une condition
typedef struct CONDITION {
	int (*conditionOK)(void *data, void *value); //Fonction a appeler avec leux deux paramètres du dessous. Retourne vrai si la condition est verifiée
	void * data; //Pointeur vers la donnée du capteur a tester
	int value; //Valeur a tester
	struct CONDITION * nextCondition; //Pointeur vers la condition suivante
} CONDITION;

//Une règle
typedef struct RULE{
	struct CONDITION * conditions;
	void * action;
	struct RULE * nextRule;
} RULE;







