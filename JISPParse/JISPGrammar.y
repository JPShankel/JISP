%{

#pragma warning (disable:4129) //unrecognized escape sequence in bison code


void yyerror(char *);
int yylex();

int jisp_parse_line;
extern int jisp_lex_line;

const char *GetToken(unsigned int index);


%}

%start list

%token INTEGER
%token FLOAT
%token RATIONAL
%token IDENTIFIER
%token STRING
%token BOOLEAN
%token CHARACTER

%token OPENPAREN
%token CLOSEDPAREN

%token QUOTE


%%

list: OPENPAREN list_elements CLOSEDPAREN {$$=$2;printf("list %s\n",GetToken($2));}
| OPENPAREN list CLOSEDPAREN {$$=$2;printf("list %s\n",GetToken($2));}
;

list_elements : list_element {$$=$1;printf("list element: %s\n",GetToken($1));}
| list_elements list_element {$$=$2;printf("list elements: %s\n",GetToken($1));}
;

list_element : terminal_element {$$=$1;printf("terminal element: %s\n",GetToken($1));}
| list {$$=$1;printf("list in list element: %s\n",GetToken($1));}
;

terminal_element : number {$$=$1;printf("number %s\n",GetToken($1));}
| STRING {$$=$1;printf("string: %s\n",GetToken($1));}
| BOOLEAN {$$=$1;printf("bool: %s\n",GetToken($1));}
| QUOTE terminal_element {$$=$1;printf("quoted %s\n",GetToken($2));}
;

number: INTEGER {$$=$1;printf("integer: %s\n",GetToken($1));}
| FLOAT {$$=$1;printf("float: %s\n",GetToken($1));}
| RATIONAL {$$=$1;printf("fraction: %s\n",GetToken($1));}
| IDENTIFIER {$$=$1;printf("identifier: %s\n",GetToken($1));}
| CHARACTER {$$=$1;printf("character: %s\n",GetToken($1));}
;


%%

