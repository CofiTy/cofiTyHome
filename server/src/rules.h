// ########################### Règles ################################


void applyRules();

extern int testEqual(int * data, int value);
extern int testGreaterOrEqual(int * data, int value);
extern int testLessOrEqual(int * data, int value);
extern int testGreater(int * data, int value);
extern int testLess(int * data, int value);

//Premier element liste regles
struct RULE * startRule;

//Une condition
typedef struct CONDITION {
	int (*conditionOK)(int *, int); //Fonction a appeler avec leux deux paramètres du dessous. Retourne vrai si la condition est verifiée
	int * data; //Pointeur vers la donnée du capteur a tester
	int value; //Valeur a tester
	struct CONDITION * nextCondition; //Pointeur vers la condition suivante
};

//Une règle
typedef struct RULE{
	struct CONDITION * conditions;
	void * action;
	struct RULE * nextRule;
};







