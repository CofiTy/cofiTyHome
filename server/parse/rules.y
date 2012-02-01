%{
    #include <math.h>
    #include <stdio.h>
    #include <string.h>
    #include <stdlib.h>

    #include "../src/rules.h"
    #include "../src/sensors.h"
    #include "../src/actions.h"
    #include "../src/actionneurs.h"

    void yyerror(char * msg) {
      fprintf(stderr, "Problème lors du parsage d'un des fichiers !! : %s\n", msg);
    }

    //-- Lexer prototype required by bison, aka getNextToken()
    int yylex();

    FILE *yyin; /* Input for yacc parser. */
    extern void yyerror(char *str); /* Our version. */
    extern int yywrap(void); /* Our version. */
    extern int yylex(void); /* Lexical analyzer function. */
    extern int yyparse(void); /* Parser function. */


    struct rule_t * currentRule;
    struct condition_t * currentCondition;

    struct sensorType * currentSensor;

    struct actionneur_t * currentActionneur;

    struct action_t * currentAction;
    struct actionFct_t * currentActionFct;

%}

%union  { char chaine[256]; int valeur; }

%start root

%token <valeur> CINTERRUPTEUR
%token <valeur> CPRESENCE
%token <valeur> CTEMPERATURE
%token <valeur> CCONTACT

%token <valeur> CCOURRANT

%token <valeur> NOMACTION
%token <valeur> ACTIONNEURS
%token <valeur> POINT

%token <chaine> RULE
%token <chaine> CONDITIONS
%token <chaine> ACTIONS
%token <chaine> EQUAL
%token <chaine> GREATEROREQUAL
%token <chaine> LESSOREQUAL
%token <chaine> GREATER
%token <chaine> LESS
%token <chaine> IDENTIFIER
%token <valeur> NUMBER

 
%type <chaine> root 
%type <chaine> parseSensors oneSensor initSensor typeSensor
%type <chaine> parseActionneurs oneActionneur initActionneur typeActionneur
%type <chaine> parseActions oneAction actionId someActionneursFct oneActionneurFct
%type <chaine> parseRules onerule someconditions someactions operator ruleid onecondition conditionid
 
%%

root:
        parseSensors
        | parseActionneurs
        | parseActions
        | parseRules
;

/*************** Capteurs ***************/

parseSensors:
        oneSensor
        | oneSensor parseSensors
;

oneSensor:
        initSensor typeSensor IDENTIFIER
{
    printf("Sensor %s\n", $3);
    memcpy(currentSensor->id, "\0\0\0\0\0\0\0\0\0", sizeof(char) * 9);
    strcpy(currentSensor->id, $3);
};
        | initSensor typeSensor NUMBER
{
    printf("Sensor %d\n", $3);
    memcpy(currentSensor->id, "\0\0\0\0\0\0\0\0\0", sizeof(char) * 9);
    sprintf(currentSensor->id, "%d", $3);

};

initSensor:
{
    struct sensorType * old = currentSensor;

    currentSensor = calloc(1, sizeof(struct sensorType));

    if(sensors == 0){
        sensors = currentSensor;
    } else {
        old->nextSensor = currentSensor;
    }
};

typeSensor:
        CINTERRUPTEUR
{
    currentSensor->type = INTERRUPTEUR;

    currentSensor->data = malloc(sizeof(dataINTERRUPTEUR));
    memset(currentSensor->data, 0, sizeof(dataINTERRUPTEUR));

    currentSensor->decode = decodeInterrupteur;
};
        | CPRESENCE
{
    currentSensor->type = PRESENCE;

    currentSensor->data = malloc(sizeof(dataPRESENCE));
    memset(currentSensor->data, 0, sizeof(dataPRESENCE));

    currentSensor->decode = decodePresence;
};
        | CTEMPERATURE
{
    currentSensor->type = TEMPERATURE;

    currentSensor->data = malloc(sizeof(dataTEMPERATURE));
    memset(currentSensor->data, 0, sizeof(dataTEMPERATURE));

    currentSensor->decode = decodeTemperature;
};
        | CCONTACT
{
    currentSensor->type = CONTACT;

    currentSensor->data = malloc(sizeof(dataCONTACT));
    memset(currentSensor->data, 0, sizeof(dataCONTACT));

    currentSensor->decode = decodeContact;
};

/*************** Actionneurs ***************/

parseActionneurs:
        oneActionneur
        | oneActionneur parseActionneurs
;

oneActionneur:
        initActionneur typeActionneur IDENTIFIER
{
    printf("Actionneur %s\n", $3);
    memcpy(currentActionneur->id, "\0\0\0\0\0\0\0\0\0", sizeof(char) * 9);
    strcpy(currentActionneur->id, $3);
};
        | initActionneur typeActionneur NUMBER
{
    printf("Actionneur %d\n", $3);
    memcpy(currentActionneur->id, "\0\0\0\0\0\0\0\0\0", sizeof(char) * 9);
    sprintf(currentActionneur->id, "%d", $3);
};

initActionneur:
{
    struct actionneur_t * old = currentActionneur;

    currentActionneur = calloc(1, sizeof(struct actionneur_t));

    if(actionneurs == 0){
        actionneurs = currentActionneur;
    } else {
        old->nextActionneur = currentActionneur;
    }

};

typeActionneur:
        CCOURRANT
{
    currentSensor->type = COURRANT;
};

/*************** Actions ***************/

parseActions:
        oneAction
        | oneAction parseActions
;

oneAction:
        NOMACTION actionId ACTIONNEURS someActionneursFct
{
    printf("Action %s\n", $2);
};

actionId:
        IDENTIFIER
{
    struct action_t * old = currentAction;

    currentAction = calloc(1, sizeof(struct action_t));

    if(actions == 0){
        actions = currentAction;
    } else {
        old->nextAction = currentAction;
    }

    strcpy(currentAction->nom, $1);
};

someActionneursFct:
    oneActionneurFct
    | oneActionneurFct someActionneursFct
;

oneActionneurFct:
    IDENTIFIER POINT IDENTIFIER
{
    struct actionFct_t * old = currentActionFct;

    currentActionFct = calloc(1, sizeof(struct actionFct_t));

    if(currentAction->actionFcts == 0){
        currentAction->actionFcts = currentActionFct;
    } else {
        old->nextActionFct = currentActionFct;
    }

    currentActionFct->actionneur = getActionneur($1);
    setActionneurFct(currentActionFct, $3);
};


/*************** Règles ***************/

parseRules:
    onerule
    | parseRules onerule
;

onerule:
	ruleid CONDITIONS someconditions ACTIONS someactions
{
};


ruleid:
        RULE IDENTIFIER
{
    printf("rule  %s\n", $2);

    struct rule_t * old = currentRule;

    currentRule = calloc(1, sizeof(struct rule_t));

    if(startRule == 0){
        startRule = currentRule;
    } else {
        old->nextRule = currentRule;
    }
};

someconditions:
        onecondition
        | someconditions onecondition

onecondition:
	conditionid operator numberid

conditionid:
        IDENTIFIER
{
    printf("\tcond  %s\n", $1);

    struct condition_t * old = currentCondition;

    currentCondition = calloc(1, sizeof(struct condition_t));

    if(currentRule->conditions == 0){
            currentRule->conditions = currentCondition;
    } else {
            old->nextCondition = currentCondition;
    }
    char t[9];
    memset(t, '\0', sizeof (char) * 9);
    strcpy(t, $1);
    currentCondition->data = getSensor(t)->data;
};
        | IDENTIFIER POINT IDENTIFIER
{
    printf("\tcond  %s\n", $1);

    struct condition_t * old = currentCondition;

    currentCondition = calloc(1, sizeof(struct condition_t));

    if(currentRule->conditions == 0){
            currentRule->conditions = currentCondition;
    } else {
            old->nextCondition = currentCondition;
    }
    /*char t[9];
    memcpy(t, "\0\0\0\0\0\0\0\0", sizeof(char) * 9);
    strcpy(t, $1);
    currentCondition->data = getSensor(t)->data;*/

    setConditionName(currentCondition, $1, $3);
};

numberid:
    IDENTIFIER
{
    currentCondition->value = atoi($1);
};
    | NUMBER
{
    currentCondition->value = $1;
};

operator:
	EQUAL
{
    currentCondition->conditionOK = testEqual;
};
	| GREATEROREQUAL
{
    currentCondition->conditionOK = testGreaterOrEqual;
};
	| LESSOREQUAL
{
    currentCondition->conditionOK = testLessOrEqual;
};
	| GREATER
{
    currentCondition->conditionOK = testGreater;
};
	| LESS
{
    currentCondition->conditionOK = testLess;
};

someactions:
	IDENTIFIER
{
    currentRule->action = getAction($1);
};
	| IDENTIFIER someactions

 
%%

void parseSensors() {
	printf("%s\n", "Parsing Sensors..");

	yyin = fopen( "config/sensors", "r" );

	yyparse();
}

void parseActionneurs() {
	printf("\n%s\n", "Parsing Actionneurs..");

	yyin = fopen( "config/actionneurs", "r" );

	yyparse();
}

void parseActions() {
	printf("\n%s\n", "Parsing Actions..");

	yyin = fopen( "config/actions", "r" );

	yyparse();
}

void parseRules() {
	printf("\n%s\n", "Parsing Rules..");

	yyin = fopen( "config/rules", "r" );

	yyparse();
}

void parseAll() {
	printf("\n%s\n", "Start Parsing..");

	parseSensors();
        parseActionneurs();
        parseActions();
        parseRules();
}

