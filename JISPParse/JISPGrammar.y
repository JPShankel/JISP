%{

#pragma warning (disable:4129) //unrecognized escape sequence in bison code


void yyerror(char *);
int yylex();

int jisp_parse_line;
extern int jisp_lex_line;


%}

%start statement_list

%token NUM
%token STRING




%%


statement_list : statement
| statement_list statement
;


statement : NUM
| STRING
;


%%

