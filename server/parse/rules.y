%{
    #include <math.h>
    #include <stdio.h>
    #include <string.h>
    #include <stdlib.h>

    #include "../src/rules.h"
    #include "../src/sensors.h"
    #include "../src/actions.h"
    #include "../src/actionneurs.h"
    #include "../src/common.h"
    
    #include "../../kernel/memory/memory.h"

    void yyerror(char * msg) {
      fprintf(stderr, "Problème lors du parsage d'un des fichiers !! : %s\n", msg);

        //TODO:appeler clean
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
    
    int flagTime = FALSE;

    typedef enum state {
    INIT = 0,
    RELOADING = 1
    } state;

    state progState = INIT;

%}

%union  { char chaine[256]; int valeur; }

%start root

%token <valeur> CINTERRUPTEUR
%token <valeur> CPRESENCE
%token <valeur> CTEMPERATURE
%token <valeur> CCONTACT
%token <valeur> CHORLOGE

%token <valeur> CCOURRANT
%token <valeur> CCHAUFFAGE
%token <valeur> CCAFFE
%token <valeur> CVOLETS

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

%token <chaine> NAMELOGRULES
%token <chaine> NAMELOGSENSORS

%token <chaine> CONNECT
%token <chaine> LISTEN
%token <chaine> IP
%token <chaine> COLUMN

%token <chaine> IDENTIFIER
%token <valeur> NUMBER

%token <chaine> TIMEGREATERTHAN
%token <chaine> TIMELOWERTHAN
 
%type <chaine> root 
%type <chaine> parseSensors oneSensor initSensor typeSensor idSensor nameSensor
%type <chaine> parseActionneurs oneActionneur initActionneur typeActionneur idActionneur nameActionneur
%type <chaine> parseActions oneAction actionId someActionneursFct oneActionneurFct
%type <chaine> parseRules onerule someconditions someactions operator ruleid onecondition conditionid
%type <chaine> parseConfig 
 
%%

root:
        parseSensors
        | parseActionneurs
        | parseActions
        | parseRules
        | parseConfig
;

/*************** Capteurs ***************/

parseSensors:
        oneSensor
        | oneSensor parseSensors
;

oneSensor:
        initSensor typeSensor idSensor nameSensor
;

idSensor:
        IDENTIFIER
{
    printf("Sensor %s\n", $1);
    memset(currentSensor->id, '\0', sizeof(char) * SIZE_ID);
    strcpy(currentSensor->id, $1);
};
        | NUMBER
{
    printf("Sensor %d\n", $1);
    memset(currentSensor->id, '\0', sizeof(char) * SIZE_ID);
    sprintf(currentSensor->id, "%d", $1);

};

nameSensor:
{};
        | IDENTIFIER
{
    memset(currentSensor->name, '\0', sizeof(char) * SIZE_NAME);
    strcpy(currentSensor->name, $1);
};


initSensor:
{
    struct sensorType * old = currentSensor;

    if((currentSensor = gMalloc(sizeof(struct sensorType))) == NULL)
    {
		fprintf(stderr, "No Memory\n");
        exit(ERROR);
    }
    memset(currentSensor, 0, sizeof(struct sensorType));

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

    if((currentSensor->data = gMalloc(sizeof(dataINTERRUPTEUR))) == NULL)
    {
		fprintf(stderr, "No Memory\n");
        exit(ERROR);
    }
    memset(currentSensor->data, 0, sizeof(dataINTERRUPTEUR));

    currentSensor->decode = decodeInterrupteur;
};
        | CPRESENCE
{
    currentSensor->type = PRESENCE;


    if((currentSensor->data = gMalloc(sizeof(dataPRESENCE))) == NULL)
    {
		fprintf(stderr, "No Memory\n");
        exit(ERROR);
    }
    memset(currentSensor->data, 0, sizeof(dataPRESENCE));

    currentSensor->decode = decodePresence;
};
        | CTEMPERATURE
{
    currentSensor->type = TEMPERATURE;

    if((currentSensor->data = gMalloc(sizeof(dataTEMPERATURE))) == NULL)
    {
		fprintf(stderr, "No Memory\n");
        exit(ERROR);
    }
    memset(currentSensor->data, 0, sizeof(dataTEMPERATURE));

    currentSensor->decode = decodeTemperature;
};
        | CCONTACT
{
    currentSensor->type = CONTACT;

    if((currentSensor->data = gMalloc(sizeof(dataCONTACT))) == NULL)
    {
		fprintf(stderr, "No Memory\n");
        exit(ERROR);
    }
    memset(currentSensor->data, 0, sizeof(dataCONTACT));

    currentSensor->decode = decodeContact;
};
       | CHORLOGE
{
    currentSensor->type = HORLOGE;

    if((currentSensor->data = gMalloc(sizeof(dataHORLOGE))) == NULL)
    {
		fprintf(stderr, "No Memory\n");
        exit(ERROR);
    }
    memset(currentSensor->data, 0, sizeof(dataHORLOGE));

    currentSensor->decode = decodeHorloge;
};




/*************** Actionneurs ***************/

parseActionneurs:
        oneActionneur
        | oneActionneur parseActionneurs
;

oneActionneur:
        initActionneur typeActionneur idActionneur nameActionneur
;

initActionneur:
{
    struct actionneur_t * old = currentActionneur;

    if((currentActionneur = gMalloc(sizeof(struct actionneur_t))) == NULL)
    {
		fprintf(stderr, "No Memory\n");
        exit(ERROR);
    }
    memset(currentActionneur, 0, sizeof(struct actionneur_t));

    if(actionneurs == 0){
        actionneurs = currentActionneur;
    } else {
        old->nextActionneur = currentActionneur;
    }

};

typeActionneur:
        CCOURRANT
{
    currentActionneur->type = COURRANT;
};
        |CCAFFE
{
    currentActionneur->type = CAFFE;
};
        |CVOLETS
{
    currentActionneur->type = VOLETS;
};
        |CCHAUFFAGE
{
    currentActionneur->type = CHAUFFAGE;
};

idActionneur:
        IDENTIFIER
{
    printf("Actionneur %s\n", $1);
    memset(currentActionneur->id, '\0', sizeof(char) * SIZE_ID);
    strcpy(currentActionneur->id, $1);
};
        | NUMBER
{
    printf("Actionneur %d\n", $1);
    memset(currentActionneur->id, '\0', sizeof(char) * SIZE_ID);
    sprintf(currentActionneur->id, "%d", $1);
};

nameActionneur:
{};
        | IDENTIFIER
{
    memset(currentActionneur->name, '\0', sizeof(char) * SIZE_NAME);
    strcpy(currentActionneur->name, $1);
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

    if((currentAction = gMalloc(sizeof(struct action_t))) == NULL)
    {
		fprintf(stderr, "No Memory\n");
        exit(ERROR);
    }
    memset(currentAction, 0, sizeof(struct action_t));

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

    if((currentActionFct = gMalloc(sizeof(struct actionFct_t))) == NULL)
    {
		fprintf(stderr, "No Memory\n");
        exit(ERROR);
    }
    memset(currentActionFct, 0, sizeof(struct actionFct_t));

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

    if((currentRule = gMalloc(sizeof(struct rule_t))) == NULL)
    {
		fprintf(stderr, "No Memory\n");
        exit(ERROR);
    }
    memset(currentRule, 0, sizeof(struct rule_t));

    strcpy(currentRule->name, $2);

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

    if((currentCondition = gMalloc(sizeof(struct condition_t))) == NULL)
    {
		fprintf(stderr, "No Memory\n");
        exit(ERROR);
    }
    memset(currentCondition, 0, sizeof(struct condition_t));

    if(currentRule->conditions == 0){
            currentRule->conditions = currentCondition;
    } else {
            old->nextCondition = currentCondition;
    }
    char t[SIZE_NAME];
    memset(t, '\0', sizeof (char) * SIZE_NAME);
    strcpy(t, $1);
    
    currentCondition->data = getSensor(t)->data;
};
        | IDENTIFIER POINT IDENTIFIER
{
    printf("\tcond  %s\n", $1);

    struct condition_t * old = currentCondition;

    if((currentCondition = gMalloc(sizeof(struct condition_t))) == NULL)
    {
        fprintf(stderr, "No Memory\n");
        exit(ERROR);
    }
    memset(currentCondition, 0, sizeof(struct condition_t));

    if(currentRule->conditions == 0){
            currentRule->conditions = currentCondition;
    } else {
            old->nextCondition = currentCondition;
    }
    /*char t[SIZE_NAME];
    memcpy(t, "\0\0\0\0\0\0\0\0", sizeof(char) * SIZE_NAME);
    strcpy(t, $1);
    currentCondition->data = getSensor(t)->data;*/

    setConditionName(currentCondition, $1, $3);
};

numberid:
    IDENTIFIER
{
	if(flagTime)
	{
        int i;

        int hi = 0;
        char h[3] = {'\0'};
        int mi = 0;
        char m[3] = {'\0'};

        struct tm str_time;
        time_t currentTime;

        if(strlen($1) > 5)
        {
            yyerror("Temps Invalide!\n");
        }

        for(i = 0;i < strlen($1); i++)
        {
            if($1[i] == 'h' || $1[i] == 'H'){
                hi = 42;
                continue;
            }
            if(hi < 2)
            {
                h[hi++] = $1[i];
            }
            else
            {
                if(mi < 2)
                {
                    m[mi++] = $1[i];
                }
            }
        }

        hi = atoi(h);
        mi = atoi(m);

        if(hi < 0  || hi > 23
            || mi < 0 || mi > 59)
        {
            yyerror("Temps Invalide!\n");
        }

        currentTime = time (NULL);
        memcpy(&str_time, localtime(&currentTime), sizeof(struct tm));

        str_time.tm_hour = hi;
        str_time.tm_min = mi;
        str_time.tm_sec = 0;

        currentCondition->value = mktime(&str_time);
	}
	else
	{
		currentCondition->value = atoi($1);
	}
	flagTime = FALSE;
    
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
	| TIMEGREATERTHAN
{
    currentCondition->conditionOK = testTimeMore;
    flagTime = TRUE;
};
	| TIMELOWERTHAN
{
    currentCondition->conditionOK = testTimeLess;
    flagTime = TRUE;
};

someactions:
	IDENTIFIER
{
    currentRule->action = getAction($1);
};
	| IDENTIFIER someactions

/********************** Config **************************/

parseConfig:
           CONNECT IP COLUMN IDENTIFIER LISTEN IDENTIFIER NAMELOGRULES IDENTIFIER NAMELOGSENSORS IDENTIFIER
           {
            /*
            printf("### Original ###\n");
            printf("\tConnect to IP: %s on Port: %s\n",$2, $4);
            printf("\tListen on Port: %s\n", $6);
            */
            if((conIP = gMalloc(strlen($2)*sizeof(char))) == NULL)
            {
                fprintf(stderr, "No Memory!\n");
                exit(ERROR);
            }
            memcpy(conIP, $2, strlen($2));

            conPort = atoi($4);
            lisPort = atoi($6);
            
            if((nameLogSensors =
            gMalloc((strlen($10)+strlen(LOG_EXT))*sizeof(char))) == NULL)
            {
                fprintf(stderr, "No Memory!\n");
                exit(ERROR);
            }
            memcpy(nameLogSensors, $10, strlen($10));
            strcat(nameLogSensors, LOG_EXT);
            
            if((nameLogRules =
            gMalloc((strlen($8)+strlen(LOG_EXT))*sizeof(char))) == NULL)
            {
                fprintf(stderr, "No Memory!\n");
                exit(ERROR);
            }
            memcpy(nameLogRules, $8, strlen($8));
            strcat(nameLogRules, LOG_EXT);
            
            /*printf("### Copied ###\n");*/
            printf("\tConnect to IP: %s on Port: %d\n", conIP, conPort);
            printf("\tListen on Port: %d\n", lisPort);
            
            printf("\tLog Rules: %s\n\tLog Sensors: %s\n", nameLogRules, nameLogSensors);
           };
 
%%

void parseSensors() {
	printf("%s\n", "Parsing Sensors..");

	if((yyin = fopen( "server/config/sensors", "r" )) == NULL)
	{
		printf("ERROR: No File server/config/sensors...");
		exit(ERROR);
	}
	
    yyparse();
}

void parseActionneurs() {
	printf("\n%s\n", "Parsing Actionneurs..");

	if((yyin = fopen( "server/config/actionneurs", "r" )) == NULL)
	{
		printf("ERROR: No File server/config/actionneurs...");
		exit(ERROR);
	}

    yyparse();
}

void parseActions() {
	printf("\n%s\n", "Parsing Actions..");

	if((yyin = fopen( "server/config/actions", "r" )) == NULL)
	{
		printf("ERROR: No File server/config/actions...");
		exit(ERROR);
	}

    yyparse();
}

void parseRules() {
	printf("\n%s\n", "Parsing Rules..");

	if((yyin = fopen( "server/config/rules", "r" )) == NULL)
	{
		printf("ERROR: No File server/config/rules...\n");
		exit(ERROR);
	}

	pthread_mutex_lock(&sensorsMutex);

    yyparse();

    pthread_mutex_unlock(&sensorsMutex);
}

void parseConfig() {
    printf("\n%s\n", "Parsing Config...");

    if((yyin = fopen("server/config/config", "r")) == NULL)
    {
		printf("ERROR: No File server/config/config...\n");
		exit(ERROR);
	}
		
    yyparse();

}

void parseAll() {
	printf("\n%s\n", "Start Parsing..");

//TODO:Changer en parseFile.
	parseSensors();
        parseActionneurs();
        parseActions();
        parseRules();
        parseConfig();
}

void parseFile(const char* file, state progState){
    
}

//bool reparseFiles(enumBenJ p, const char * file){
int reparseFiles(int p, const char * file) {
//TODO:on clean la memoire
//TODO:faire les parse qu'il faut.
}

void clean(state progState){

    //TODO:on clean la memoire

    if(progState == INIT){
        exit(-1);
    } else if(progState == RELOADING){
        //parseAll
    }
}

void getHistory(int id, int nbValues, struct json_object* message){
    parseFile(nameLogRules, RELOADING);
}

