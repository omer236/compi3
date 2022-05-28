%{
/* Declarations section */
	#include "hw3_output.hpp"
	#include "Expression.h"
	#include "parser.tab.hpp"
	using namespace output;
	#define YY_USER_ACTION yylloc.first_line = yylloc.last_line = yylineno;
%}

%option yylineno
%option noyywrap
%option nounput

whitespace		([\t\n\r ])

%%
{whitespace}						;
\/\/[^\r\n]*[\r|\n|\r\n]?			;
void								return VOID;
int 								return INT;
byte 								return BYTE;
b 									return B;
bool 								return BOOL;
auto								return AUTO;
and 								return AND;
or 									return OR;
not 								return NOT;
true 								{yylval = new Expression(yytext,"BOOL"); return TRUE;}
false 								{yylval = new Expression(yytext,"BOOL"); return FALSE;}
return 								return RETURN;
if 									return IF;
else 								return ELSE;
while 								return WHILE;
break 								return BREAK;
continue 							return CONTINUE;
;									return SC;
,									return COMMA;
\(									return LPAREN;
\)									return RPAREN;
\{									return LBRACE;
\}									return RBRACE;
\=									return ASSIGN;
==|!=								return EQUAL;
\<|\>|<=|>=							return RELOP;
\*|\/                               return MULTDIV;
\+|\-                               return PLUSMINUS;
[a-zA-Z][a-zA-Z0-9]*				{yylval = new Expression(yytext,""); return ID;}
0|[1-9][0-9]*						{yylval = new Expression(yytext,"INT"); return NUM;}
\"([^\n\r\"\\]|\\[rnt"\\])+\" 		{yylval = new Expression(yytext,"STRING"); return STRING;}
.									{errorLex(yylloc.first_line); exit(1);}

%%