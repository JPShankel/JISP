%{
#pragma warning (disable: 4786)
#include "JISPGrammar.h"
#include <stdlib.h>
#include <string.h>
#pragma warning (disable:4129) //unrecognized escape sequence in bison code

int YYInput(char *buf,int max_size);
int AddToken(const char *token);
int in_comment = 0,jisp_lex_line=0;
extern int jisp_parse_line;
#undef YY_INPUT
#define YY_INPUT(buf,result,max_size) (result = YYInput(buf,max_size))


%}
%%


[0-9]+ {if (in_comment) REJECT;yylval = AddToken(yytext);return NUM; }

[0-9]+[.][0-9]+ {if (in_comment) REJECT;yylval = AddToken(yytext);return NUM;}
[.][0-9]+ {if (in_comment) REJECT;yylval = AddToken(yytext);return NUM;}

[A-Za-z][A-Za-z0-9_]* {if (in_comment) REJECT;yylval = AddToken(yytext);return STRING;}

. { if(in_comment) REJECT; }
%%

static char *inputStream;

int yywrap()
{
	return 1;
}
int yyparse();

int YYInput(char *buf,int max_size)
{
	int n = min(max_size,(int)strlen(inputStream));
	if (n > 0)
	{
		memcpy(buf,inputStream,n);
		inputStream += n;
	}
	return n;
}

void compile(char *str)
{
	in_comment = 0;
	jisp_lex_line = 0;
	jisp_parse_line = -1;
	inputStream = str;
	yyrestart(0);
	yyparse();
}
