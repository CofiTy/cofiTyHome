%{

	#include "rules.tab.h"
  int state = 0;
	
%}

%option noyywrap

%%

^"#".*$                 {;/*Ligne de commentaires, on ignore !*/}

"INTERRUPTEUR"		{ strcpy(yylval.chaine, yytext); return CINTERRUPTEUR; }
"PRESENCE"		{ strcpy(yylval.chaine, yytext); return CPRESENCE; }
"TEMPERATURE"		{ strcpy(yylval.chaine, yytext); return CTEMPERATURE; }
"CONTACT"		{ strcpy(yylval.chaine, yytext); return CCONTACT; }
"HORLOGE"      { strcpy(yylval.chaine, yytext); return CHORLOGE; }


"COURRANT"		{ strcpy(yylval.chaine, yytext); return CCOURRANT; }
"CAFFE"                 { strcpy(yylval.chaine, yytext); return CCAFFE; }
"VOLETS"		{ strcpy(yylval.chaine, yytext); return CVOLETS; }
"CHAUFFAGE"		{ strcpy(yylval.chaine, yytext); return CCHAUFFAGE; }
"VIRTUALKEYBOARD" 	{ strcpy(yylval.chaine, yytext); return CVKB; }
"MYSTERE" 	{ strcpy(yylval.chaine, yytext); return CMYSTERE; }


[Rr]"ule:"		{ strcpy(yylval.chaine, yytext); return RULE; }
[Cc]"onditions:"	{ strcpy(yylval.chaine, yytext); return CONDITIONS; }
[Aa]"ctions:"		{ strcpy(yylval.chaine, yytext); return ACTIONS; }

[Nn]"omAction:"         { strcpy(yylval.chaine, yytext); return NOMACTION; }
[Aa]"ctionneurs:"       { strcpy(yylval.chaine, yytext); return ACTIONNEURS; }

[Nn]"ame"[Ll]"og"[Rr]"ules:"	{ strcpy(yylval.chaine, yytext); return NAMELOGRULES; }
[Nn]"ame"[Ll]"og"[Ss]"ensors:"	{ strcpy(yylval.chaine, yytext); return NAMELOGSENSORS; }

([0-9]{1,3}\.){3}[0-9]{1,3} {strcpy(yylval.chaine, yytext); return IP;}

\.                      { strcpy(yylval.chaine, yytext); return POINT; }

"=="			{ strcpy(yylval.chaine, yytext); return EQUAL; }
">="			{ strcpy(yylval.chaine, yytext); return GREATEROREQUAL; }
"<="			{ strcpy(yylval.chaine, yytext); return LESSOREQUAL; }
">"			{ strcpy(yylval.chaine, yytext); return GREATER; }
"<"			{strcpy(yylval.chaine, yytext); return LESS; }

"-gt"	{ strcpy(yylval.chaine, yytext); return TIMEGREATERTHAN; }
"-lt"	{ strcpy(yylval.chaine, yytext); return TIMELOWERTHAN; }

[Cc]"onnect"[Ss]"erver:"    {strcpy(yylval.chaine, yytext); return CONNECT; }
[Ll]"isten"[Cc]"lient:"     {strcpy(yylval.chaine, yytext); return LISTEN; }

":"  {strcpy(yylval.chaine, yytext); return COLUMN;}

[0-9a-zA-Z_]*           { strcpy(yylval.chaine, yytext); return IDENTIFIER; }
[0-9]+			{ yylval.valeur = atoi(yytext); return NUMBER; }

[ \t]+			{ ; /* espaces vierges */ }
(\r?\n)			{ ; /* ligne vide : reconnait les CR-LF (windows) et les LF (unix) */ }
"//".*\n		{ ; /* commentaires */ }
.			{ ; /*cerr << "Unrecognized token!" << endl; exit(1); */ }

%%
