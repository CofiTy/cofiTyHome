%{
    #include <math.h>
    #include <stdio.h>
    #include <string.h>
    #include <stdlib.h>

    #include "rules.h"
    #include "sensors.h"

    void yyerror(char * msg) {
      fprintf(stderr, "%s\n", msg);
    }

    

    //-- Lexer prototype required by bison, aka getNextToken()
    int yylex();

    FILE *yyin; /* Input for yacc parser. */
    extern void yyerror(char *str); /* Our version. */
    extern int yywrap(void); /* Our version. */
    extern int yylex(void); /* Lexical analyzer function. */
    extern int yyparse(void); /* Parser function. */


    struct RULE * currentRule;
    struct CONDITION * currentCondition;

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
 
%type  <chaine> root onerule someconditions someactions operator ruleid onecondition conditionid
 
%%
 
root:
    onerule
    | root onerule
;

onerule:
	ruleid CONDITIONS someconditions ACTIONS someactions
{
};


ruleid:
        RULE IDENTIFIER
{
    printf("rule  %s\n", $2);

    struct RULE * old = currentRule;

    currentRule = calloc(1, sizeof(RULE));

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

    struct CONDITION * old = currentCondition;

    currentCondition = calloc(1, sizeof(struct CONDITION));

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
    NUMBER
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

parse() {
	printf("%s\n", "start");

	yyin = fopen( "config/rules", "r" );
	//yylex();
	yyparse();
}

