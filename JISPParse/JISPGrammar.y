%{

#pragma warning (disable:4129) //unrecognized escape sequence in bison code


void yyerror(char *);
int yylex();

int jisp_parse_line;
extern int jisp_lex_line;

const char *GetToken(unsigned int index);
unsigned int AddJISPElement(unsigned int type, unsigned int dataIndex);
unsigned int AddQuotedElement(unsigned int type, unsigned int dataIndex);

unsigned int AddJISPElementToList(unsigned int elist, unsigned int element);
unsigned int BeginJISPElementList(unsigned int element);
unsigned int EndJISPElementList(unsigned int list);

unsigned int NullJISPList();

const unsigned int jleTypeCharacter_k = 0;
const unsigned int jleTypeString_k= 1;
const unsigned int jleTypeInteger_k=2 ;
const unsigned int jleTypeRational_k= 3;
const unsigned int jleTypeFloat_k= 4;
const unsigned int jleTypeComplex_k= 5;
const unsigned int jleTypeQuoted_k= 6;
const unsigned int jleTypeBoolean_k= 7;
const unsigned int jleTypeIdentifier_k= 8;
const unsigned int jleTypeList_k= 9;
const unsigned int jleTypeUnknown_k =10;

%}

%start statement

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

statement: list {$$=$1;}
| QUOTE statement {$$=AddJISPElement(jleTypeQuoted_k,$2);}
| list_element {$$=$1;}
;

list: OPENPAREN list_elements CLOSEDPAREN {$$=EndJISPElementList($2);}
| OPENPAREN CLOSEDPAREN { $$=NullJISPList(); }
;

list_elements : list_element {$$=BeginJISPElementList($1);}
| list_elements list_element {$$=AddJISPElementToList($1,$2);}
;

list_element : terminal_element {$$=$1;}
| list {$$=$1;}
| QUOTE list_element {$$=AddJISPElement(jleTypeQuoted_k,$2);}
;


terminal_element : number {$$=$1;}
| STRING {$$=AddJISPElement(jleTypeString_k,$1);}
| BOOLEAN {$$=AddJISPElement(jleTypeBoolean_k,$1);}
| IDENTIFIER {$$=AddJISPElement(jleTypeIdentifier_k,$1);}
;

number: INTEGER {$$=AddJISPElement(jleTypeInteger_k,$1);}
| FLOAT {$$=AddJISPElement(jleTypeFloat_k,$1);}
| RATIONAL {$$=AddJISPElement(jleTypeRational_k,$1);}
| CHARACTER {$$=AddJISPElement(jleTypeCharacter_k,$1);}
;


%%

