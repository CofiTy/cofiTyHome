%{
    #include <math.h>
    #include <stdio.h>
    #include <string.h>
    #include <stdlib.h>

    #include "rules.h"
    #include "sensors.h"

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

%}

%union  { char chaine[256]; int valeur; }

%start root
 
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

%token <valeur> CINTERRUPTEUR
%token <valeur> CPRESENCE
%token <valeur> CTEMPERATURE
%token <valeur> CCONTACT
 
%type <chaine> root 
%type <chaine> parseSensors oneSensor initSensor
%type <chaine> parseRules onerule someconditions someactions operator ruleid onecondition conditionid
 
%%

root:
        parseSensors
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
    strcpy(currentSensor->id, $3);
};
        | initSensor typeSensor NUMBER
{
    printf("Sensor %d\n", $3);

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

    currentSensor->data = calloc(1, sizeof(dataINTERRUPTEUR));

    currentSensor->decode = decodeInterrupteur;
};
        | CPRESENCE
{
    currentSensor->type = PRESENCE;

    currentSensor->data = calloc(1, sizeof(dataPRESENCE));

    currentSensor->decode = decodePresence;
};
        | CTEMPERATURE
{
    currentSensor->type = TEMPERATURE;

    currentSensor->data = calloc(1, sizeof(dataTEMPERATURE));

    currentSensor->decode = decodeTemperature;
};
        | CCONTACT
{
    currentSensor->type = CONTACT;

    currentSensor->data = calloc(1, sizeof(dataCONTACT));

    currentSensor->decode = decodeContact;
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
    char t[8];
    memcpy(t, "\0\0\0\0\0\0\0\0", sizeof(char) * 8);
    strcpy(t, $1);
    currentCondition->data = getSensor(t)->data;
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

};
	| IDENTIFIER someactions

 
%%

parseSensors() {
	printf("%s\n", "Parsing Sensors..");

	yyin = fopen( "config/sensors", "r" );

	yyparse();
}

parseRules() {
	printf("%s\n", "Parsing Rules..");

	yyin = fopen( "config/rules", "r" );
	yyparse();
}

parseAll() {
	printf("%s\n", "Start Parsing..");

	parseSensors();
        parseRules();
}

