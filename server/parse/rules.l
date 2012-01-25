%{

	#include "rules.tab.h"
	
%}

%option noyywrap

%%

^"#".*$                 {;/*Ligne de commentaires, on ignore !*/}

"INTERRUPTEUR"		{ strcpy(yylval.chaine, yytext); return CINTERRUPTEUR; }
"PRESENCE"		{ strcpy(yylval.chaine, yytext); return CPRESENCE; }
"TEMPERATURE"		{ strcpy(yylval.chaine, yytext); return CTEMPERATURE; }
"CONTACT"		{ strcpy(yylval.chaine, yytext); return CCONTACT; }

"COURRANT"		{ strcpy(yylval.chaine, yytext); return CCOURRANT; }

[Rr]"ule:"		{ strcpy(yylval.chaine, yytext); return RULE; }
[Cc]"onditions:"	{ strcpy(yylval.chaine, yytext); return CONDITIONS; }
[Aa]"ctions:"		{ strcpy(yylval.chaine, yytext); return ACTIONS; }

[Nn]"omAction:"         { strcpy(yylval.chaine, yytext); return NOMACTION; }
[Aa]"ctionneurs:"       { strcpy(yylval.chaine, yytext); return ACTIONNEURS; }
\.                      { strcpy(yylval.chaine, yytext); return POINT; }

"=="			{ strcpy(yylval.chaine, yytext); return EQUAL; }
">="			{ strcpy(yylval.chaine, yytext); return GREATEROREQUAL; }
"<="			{ strcpy(yylval.chaine, yytext); return LESSOREQUAL; }
">"			{ strcpy(yylval.chaine, yytext); return GREATER; }
"<"			{ strcpy(yylval.chaine, yytext); return LESS; }




[0-9a-zA-Z_]*           { strcpy(yylval.chaine, yytext); return IDENTIFIER; }
[0-9]+			{ yylval.valeur = atoi(yytext); return NUMBER; }

[ \t]+			{ ; /* espaces vierges */ }
(\r?\n)			{ ; /* ligne vide : reconnait les CR-LF (windows) et les LF (unix) */ }
"//".*\n		{ ; /* commentaires */ }
.			{ ; /*cerr << "Unrecognized token!" << endl; exit(1); */ }

%%


