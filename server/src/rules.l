%{

	#include "rules.tab.h"
	
%}

%option noyywrap

%%

[Rr]"ule:"		{ strcpy(yylval.chaine, yytext); return RULE; }
[Cc]"onditions:"	{ strcpy(yylval.chaine, yytext); return CONDITIONS; }
[Aa]"ctions:"		{ strcpy(yylval.chaine, yytext); return ACTIONS; }

"=="			{ strcpy(yylval.chaine, yytext); return EQUAL; }
">="			{ strcpy(yylval.chaine, yytext); return GREATEROREQUAL; }
"<="			{ strcpy(yylval.chaine, yytext); return LESSOREQUAL; }
">"			{ strcpy(yylval.chaine, yytext); return GREATER; }
"<"			{ strcpy(yylval.chaine, yytext); return LESS; }

[0-9]+			{ yylval.valeur = atoi(yytext); return NUMBER; }
[a-zA-Z_][0-9a-zA-Z_]*	{ strcpy(yylval.chaine, yytext); return IDENTIFIER; }

[ \t]+			{ ; /* espaces vierges */ }
(\r?\n)			{ ; /* ligne vide : reconnait les CR-LF (windows) et les LF (unix) */ }
"//".*\n		{ ; /* commentaires */ }
.			{ /*cerr << "Unrecognized token!" << endl*/; exit(1); }

%%


