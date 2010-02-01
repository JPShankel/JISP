%{
#pragma warning (disable: 4786)
#include "JISPGrammar.h"
#include <stdlib.h>
#include <string.h>
#pragma warning (disable:4129) //unrecognized escape sequence in bison code

int YYInput(char *buf,int max_size);
unsigned int AddToken(const char *token);
int in_comment = 0,jisp_lex_line=0;
extern int jisp_parse_line;
#undef YY_INPUT
#define YY_INPUT(buf,result,max_size) (result = YYInput(buf,max_size))

%}
%%

[#][tfTF] {if (in_comment) REJECT; yylval = AddToken(yytext);return BOOLEAN;}

"#\\nul" {if (in_comment) REJECT;yylval = AddToken(yytext);return CHARACTER;}
"#\\alarm" {if (in_comment) REJECT;yylval = AddToken(yytext);return CHARACTER;}
"#\\backspace" {if (in_comment) REJECT;yylval = AddToken(yytext);return CHARACTER;}
"#\\tab" {if (in_comment) REJECT;yylval = AddToken(yytext);return CHARACTER;}
"#\\newline" {if (in_comment) REJECT;yylval = AddToken(yytext);return CHARACTER;}
"#\\linefeed" {if (in_comment) REJECT;yylval = AddToken(yytext);return CHARACTER;}
"#\\vtab" {if (in_comment) REJECT;yylval = AddToken(yytext);return CHARACTER;}
"#\\page" {if (in_comment) REJECT;yylval = AddToken(yytext);return CHARACTER;}
"#\\return" {if (in_comment) REJECT;yylval = AddToken(yytext);return CHARACTER;}
"#\\escape" {if (in_comment) REJECT;yylval = AddToken(yytext);return CHARACTER;}
"#\\space" {if (in_comment) REJECT;yylval = AddToken(yytext);return CHARACTER;}
"#\\delete" {if (in_comment) REJECT;yylval = AddToken(yytext);return CHARACTER;}

[(] {if (in_comment) REJECT;yylval = AddToken(yytext);return OPENPAREN;}
[)] {if (in_comment) REJECT;yylval = AddToken(yytext);return CLOSEDPAREN;}
['] {if (in_comment) REJECT;yylval = AddToken(yytext);return QUOTE;}

[#][\\][A-Za-z0-9_!@#$%^&*()\-+={}\\|\[\]\?><,./;:'] {if (in_comment) REJECT;yylval = AddToken(yytext);return CHARACTER;}

[0-9]+ {if (in_comment) REJECT;yylval = AddToken(yytext);return INTEGER; }

[0-9]+[/][0-9]+ {if (in_comment) REJECT; yylval = AddToken(yytext);return RATIONAL;}

[0-9]+[.][0-9]+ {if (in_comment) REJECT;yylval = AddToken(yytext);return FLOAT;}

[.][0-9]+ {if (in_comment) REJECT;yylval = AddToken(yytext);return FLOAT;}

[A-Za-z][A-Za-z0-9_\?]* {if (in_comment) REJECT;yylval = AddToken(yytext);return IDENTIFIER;}

[\"][A-Za-z0-9_ !@#$%^&*()\-+={}\\|\[\]\?><,./;:']*[\"] {if (in_comment) REJECT;yylval = AddToken(yytext);return STRING;}

[+/\-*] {if (in_comment) REJECT;yylval = AddToken(yytext);return IDENTIFIER;}

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

void compile(const char *str)
{
	in_comment = 0;
	jisp_lex_line = 0;
	jisp_parse_line = -1;
	inputStream = (char *)str;
	yyrestart(0);
	yyparse();
}
