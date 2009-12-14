%{

#pragma warning (disable:4129) //unrecognized escape sequence in bison code


void yyerror(char *);
int yylex();

int jisp_parse_line;
extern int jisp_lex_line;

const char *GetToken(unsigned int index);


%}

%start statement_list

%token INTEGER
%token FLOAT
%token RATIONAL
%token IDENTIFIER
%token STRING
%token BOOLEAN
%token CHARACTER




%%


statement_list : statement
| statement_list statement
;


statement : number
| STRING {printf("string: %s\n",GetToken($1));}
| BOOLEAN {printf("bool: %s\n",GetToken($1));}
;

number: INTEGER {printf("integer: %s\n",GetToken($1));}
| FLOAT {printf("float: %s\n",GetToken($1));}
| RATIONAL {printf("fraction: %s\n",GetToken($1));}
| IDENTIFIER {printf("identifier: %s\n",GetToken($1));}
| CHARACTER {printf("character: %s\n",GetToken($1));}
;


%%

