/****************************************************/
/* File: tiny.y                                     */
/* The TINY Yacc/Bison specification file           */
/* Compiler Construction: Principles and Practice   */
/* Kenneth C. Louden                                */
/****************************************************/
%{
#define YYPARSER /* distinguishes Yacc output from other code files */

#include "globals.h"
#include "util.h"
#include "scan.h"
#include "parse.h"

#define YYSTYPE TreeNode *
static char * savedName; /* for use in assignments */
static int savedLineNo;  /* ditto */
static int savedNumber; 
static TreeNode * savedTree; /* stores syntax tree for later return */
static int yylex(void);

%}
/* reserved words */
%token IF ELSE WHILE INT VOID RETURN 
/* multicharacter token */
%token ID NUM  
/* special symbols */
%token ASSIGN EQ NE LT LE GT GE SEMI COMMA MINUS PLUS TIMES OVER
%token LPAREN RPAREN LBRACE RBRACE LCURLY RCURLY
%token ERROR 

%nonassoc LOWER_THAN_ELSE
%nonassoc ELSE
%% /* Grammar for TINY */

program     : dec_list
                 { savedTree = $1;} 
            ;
dec_list    : dec_list dec
                 { YYSTYPE t = $1;
                   if (t != NULL)
                   { while (t->sibling != NULL)
                        t = t->sibling;
                     t->sibling = $2;
                     $$ = $1; }
                     else $$ = $2;
                 }
            | dec  { $$ = $1; }
            ;
dec         : vardec_stmt { $$ = $1; }
            | fundec_stmt { $$ = $1; }
            ;
id          : ID { savedName = copyString(tokenString); };
num         : NUM { savedNumber = atoi(tokenString); };
varnum      :	num { 
								$$ = newExpNode(NUMK);
								$$->attr.val = savedNumber;
							}
						;
vardec_stmt : type_spec id SEMI
								{
									$$ = newStmtNode(VarK);
									$$->child[0] = $1;
									$$->lineno = lineno;
               		$$->attr.name = savedName; }
						| type_spec id LBRACE varnum RBRACE SEMI 
								{ $$ = newStmtNode(VarrK);
									$$->child[0] = $1;
									$$->child[1] = $4;
									$$->lineno = lineno;
									$$->attr.name = savedName;
								}
						| ERROR { $$ = NULL; }
						;
type_spec   : INT
								{ $$ = newStmtNode(TypeK);
									$$->type = Integer;
								 }
						| VOID
								{ $$ = newStmtNode(TypeK);
									$$->type = Void;
								 }
						;
fundec_stmt :	type_spec id {
								$$ = newStmtNode(FundecK);
								$$->lineno = lineno;
								$$->attr.name = savedName;
							}
						  LPAREN params RPAREN comp_stmt
							{
								$$ = $3;
								$$->child[0] = $1;
								$$->child[1] = $5;
								$$->child[2] = $7;
							}
						;
params 			: paramlist
							{	$$ = $1; }
						| type_spec
							{ 
							 $$ = newStmtNode(SingparamK);
							 $$->attr.name = "(null)";
							 $$->child[0] = $1;
							}
						;
paramlist   : paramlist COMMA param
							{ YYSTYPE t = $1;
                   if (t != NULL)
                   { while (t->sibling != NULL)
                        t = t->sibling;
                     t->sibling = $3;
                     $$ = $1; }
                     else $$ = $3;
							}
						| param 
							{ $$ = $1; }
						;
param       : type_spec id
							{	$$ = newStmtNode(SingparamK);
								$$->child[0] = $1;
								$$->attr.name = savedName;
							}
						| type_spec id LBRACE RBRACE
							{ $$ = newStmtNode(ArrparamK);
								$$->attr.name = savedName;
								$$->child[0] = $1;
							}
						;
comp_stmt   : LCURLY local_decs stmt_list RCURLY
							{ $$ = newStmtNode(CompK);
								$$->child[0] = $2;
								$$->child[1] = $3;
							}
						;
local_decs	: local_decs vardec_stmt
							{ YYSTYPE t = $1;
                   if (t != NULL)
                   { while (t->sibling != NULL)
                        t = t->sibling;
                     t->sibling = $2;
                     $$ = $1; }
                     else $$ = $2;
							}
						| { $$ = NULL; }
						;
stmt_list   :	stmt_list stmt
							{ YYSTYPE t = $1;
                   if (t != NULL)
                   { while (t->sibling != NULL)
                        t = t->sibling;
                     t->sibling = $2;
                     $$ = $1; }
                     else $$ = $2;
							}
						| { $$ = NULL; }
						;
stmt				: exp_stmt
							{ $$ = $1; }
						| comp_stmt
							{ $$ = $1; }	
						| select_stmt
							{ $$ = $1; }	
						| iter_stmt
							{ $$ = $1; }	
						| ret_stmt
							{ $$ = $1; }	
						;
exp_stmt    : exp SEMI
							{ $$ = $1; }
						|	SEMI
							{ $$ = NULL; }
						;	
select_stmt : IF LPAREN exp RPAREN stmt %prec LOWER_THAN_ELSE
							{
								$$ = newStmtNode(IfK);
								$$->child[0] = $3;
								$$->child[1] = $5;
								$$->child[2] = NULL;
							}
						| IF LPAREN exp RPAREN stmt ELSE stmt
							{
								$$ = newStmtNode(IfelseK);
								$$->child[0] = $3;
								$$->child[1] = $5;
								$$->child[2] = $7;
							}
						;
iter_stmt   : WHILE LPAREN exp RPAREN stmt
							{
								$$ = newStmtNode(IterK);
								$$->child[0] = $3;
								$$->child[1] = $5;
							}
						;
ret_stmt    : RETURN SEMI
							{ 
								$$ = newStmtNode(RetK);
								$$->child[0] = NULL;
							}
						| RETURN exp SEMI
							{ 
								$$ = newStmtNode(RetK);
								$$->child[0] = $2;
							}
						;
exp 				: var ASSIGN exp
							{
								$$ = newStmtNode(AssignK);
								$$->child[0] = $1;
							  $$->child[1] = $3;
							}
						| simple_exp
						  {	$$ = $1; }
						;	
var         : id
							{ 
								$$ = newExpNode(IdK);
								$$->attr.name = savedName;
							}
						| id
							{ 
								$$ = newExpNode(ArrIdK);
								$$->attr.name = savedName;
							}
							LBRACE exp RBRACE
							{
								$$ = $2;
								$$->child[0] = $4;
							}
						;
simple_exp  : add_exp LE add_exp
							{
								$$ = newExpNode(OpK);
								$$->child[0] = $1;
								$$->child[1] = $3;
								$$->attr.op = LE;
							}
						| add_exp LT add_exp
							{
								$$ = newExpNode(OpK);
								$$->child[0] = $1;
								$$->child[1] = $3;
								$$->attr.op = LT;
							}
						| add_exp GT add_exp
							{
								$$ = newExpNode(OpK);
								$$->child[0] = $1;
								$$->child[1] = $3;
								$$->attr.op = GT;
							}
					  | add_exp GE add_exp
							{
								$$ = newExpNode(OpK);
								$$->child[0] = $1;
								$$->child[1] = $3;
								$$->attr.op = GE;
							}
						| add_exp NE add_exp
							{
								$$ = newExpNode(OpK);
								$$->child[0] = $1;
								$$->child[1] = $3;
								$$->attr.op = NE;
							}
						| add_exp EQ add_exp
							{
								$$ = newExpNode(OpK);
								$$->child[0] = $1;
								$$->child[1] = $3;
								$$->attr.op = EQ;
							}
						| add_exp
							{ $$ = $1; }
						;
add_exp     :	add_exp PLUS term 
                 { $$ = newExpNode(OpK);
                   $$->child[0] = $1;
                   $$->child[1] = $3;
                   $$->attr.op = PLUS;
                 }
            | add_exp MINUS term
                 { $$ = newExpNode(OpK);
                   $$->child[0] = $1;
                   $$->child[1] = $3;
                   $$->attr.op = MINUS;
                 } 
            | term { $$ = $1; }
            ;
term        : term TIMES factor 
                 { $$ = newExpNode(OpK);
                   $$->child[0] = $1;
                   $$->child[1] = $3;
                   $$->attr.op = TIMES;
                 }
            | term OVER factor
                 { $$ = newExpNode(OpK);
                   $$->child[0] = $1;
                   $$->child[1] = $3;
                   $$->attr.op = OVER;
                 }
            | factor { $$ = $1; }
            ;
factor      : LPAREN exp RPAREN
                 { $$ = $2; }
            | num
                 { $$ = newExpNode(ConstK);
                   $$->attr.val = savedNumber;
                 }
            | var { $$ = $1; }
            | call { $$ = $1; }
            ;
call				: id {
								$$ = newExpNode(CallK);
								$$->attr.name = savedName;
							}
							LPAREN args RPAREN{
								$$ = $2;
								$$->child[0] = $4;
							}
						;
args        : arg_list
							{ $$ = $1; }
						| { $$ = NULL; }
						;
arg_list    : arg_list COMMA exp
							{ YYSTYPE t = $1;
                   if (t != NULL)
                   { while (t->sibling != NULL)
                        t = t->sibling;
                     t->sibling = $3;
                     $$ = $1; }
                     else $$ = $3;
							}
						| exp
							{ $$ = $1; }
						;

%%

int yyerror(char * message)
{ fprintf(listing,"Syntax error at line %d: %s\n",lineno,message);
  fprintf(listing,"Current token: ");
  printToken(yychar,tokenString);
  Error = TRUE;
  return 0;
}

/* yylex calls getToken to make Yacc/Bison output
 * compatible with ealier versions of the TINY scanner
 */
static int yylex(void)
{ return getToken(); }

TreeNode * parse(void)
{ yyparse();
  return savedTree;
}

