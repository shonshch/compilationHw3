%{
/* Declerations Section */

#include "parser.tab.hpp"
#include "output.hpp"
#include <string.h>
#include <stdio.h>

void printError ();
%}

%option yylineno
%option noyywrap

whitespace       												(\t|\r|\n|" ")

%%
void														    {return VOID;} 
int														        {return INT;}
byte														    {return BYTE;}
b														        {return B;}
bool														    {return BOOL;}
and														        {return AND;}
or														        {return OR;}
not														        {return NOT;}
true														    {return TRUE;}
false														    {return FALSE;}
return														    {return RETURN;}
if														        {return IF;}
else														    {return ELSE;}
while														    {return WHILE;}
break														    {return BREAK;}
continue														{return CONTINUE;}
;																{return SC;} 
,																{return COMMA;}
"("																{return LPAREN;} 
")"																{return RPAREN;}
"{"																{return LBRACE;}  
"}"																{return RBRACE;}  
=												       			{return ASSIGN;}      
(<|>|"<="|">=")												    {return RELOPNA;}
("=="|"!=")													    {return RELOPL;}
(\*|\/)												    		{return BINOPH;}
(\+|\-)												   		    {return BINOPL;}
[a-zA-Z][a-zA-Z0-9]*                                            {return ID;}
0|[1-9][0-9]*                                                   { return NUM;}
\"([^\n\r\"\\]|\\[rnt"\\])+\"                                     {return STRING;}
\/\/[^\r\n]*[\r|\n|\r\n]?											;
{whitespace}													;
.																printError();


%%
/* C code Section */


void printError (){
	output::errorLex(yylineno);
	exit(0);
}