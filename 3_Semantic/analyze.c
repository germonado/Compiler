/****************************************************/
/* File: analyze.c                                  */
/* Semantic analyzer implementation                 */
/* for the TINY compiler                            */
/* Compiler Construction: Principles and Practice   */
/* Kenneth C. Louden                                */
/****************************************************/

#include "globals.h"
#include "symtab.h"
#include "util.h"
#include "analyze.h"
#include <ctype.h>

/* counter for variable memory locations */
static int location = 0;
static int sp_top = 0;
static ScopeList scopePTR;
char * scopeST[1000];
char * scopeName;
static int preComp_flag = 0;

static void symtabError(TreeNode * t, char * message){
	fprintf(listing,"Error at line: %d: %s\n", t->lineno,message);
	Error = TRUE;
}

static void typeError(TreeNode * t, char * message)
{ fprintf(listing,"Type error at line %d: %s\n",t->lineno,message);
  Error = TRUE;
}


static void io_func_insert(void)
{	
	TreeNode * func;
  TreeNode * typeSpec;
	TreeNode * param;
	TreeNode * comp;
  
	func = newStmtNode(FundecK);
	func->attr.type = 1;
	func->attr.name = "input";
	func->lineno = 0;
	
	comp = newStmtNode(CompK);
	comp->child[0] = NULL;
	comp->child[1] = NULL;
	typeSpec = newStmtNode(TypeK);
	typeSpec->type = 1;
	func->child[0] = typeSpec;
	func->child[1] = NULL;
	func->child[2] = comp;

	st_insert("global", func->attr.name, func->child[0]->type, func->lineno,location++);
	  
	func = newStmtNode(FundecK);
	func->type = 0;
	func->attr.name = "output";
	func->lineno = 0;
	
	param = newStmtNode(SingparamK);
	param->attr.name = "arg";
	param->child[0] = newStmtNode(FundecK);
	param->child[0]->attr.type = 1;

	comp = newStmtNode(CompK);
	comp->child[0] = NULL;
	comp->child[1] = NULL;
	typeSpec = newStmtNode(TypeK);
	typeSpec->attr.type = 0;
 
	func->child[0] = typeSpec;
	func->child[1] = param;
	func->child[2] = comp;

	st_insert("global", func->attr.name, func->child[0]->type, func->lineno,location++);


}
/* Procedure traverse is a generic recursive 
 * syntax tree traversal routine:
 * it applies preProc in preorder and postProc 
 * in postorder to tree pointed to by t
 */
static void traverse( TreeNode * t,
               void (* preProc) (TreeNode *),
               void (* postProc) (TreeNode *) )
{ if (t != NULL)
  { preProc(t);
    { int i;
      for (i=0; i < MAXCHILDREN; i++)
        traverse(t->child[i],preProc,postProc);
    }
    postProc(t);
    traverse(t->sibling,preProc,postProc);
  }
}

/* nullProc is a do-nothing procedure to 
 * generate preorder-only or postorder-only
 * traversals from traverse
 */
static void nullProc(TreeNode * t)
{ if (t==NULL) return;
  else return;
}

/* Procedure insertNode inserts 
 * identifiers stored in t into 
 * the symbol table 
 */
static void insertNode( TreeNode * t)
{ switch (t->nodekind)
  { case StmtK:
      switch (t->kind.stmt)
      { 
				case FundecK:
					 if (sc_find(t->attr.name) == NULL) {
							sc_insert(t->attr.name);
							scopeST[++sp_top] = t->attr.name;
					 }
					 if (st_lookup("global",t->attr.name) == NULL){
						 //if(t->child[0]->type == 0) 	t->child[0]->type = 5;
						 //else if(t->child[0]->type == 1) t->child[0]->type = 6;
					   t->scope = "global";
            st_insert("global",t->attr.name,t->child[0]->type,t->lineno,location++);
						scopeName = t->attr.name;}
           else {
						symtabError(t, "This Function is redefined");	
						}
          break;
				case SingparamK:
				 if(t->child[0]->type == 0){
						 break;
					 }
				 if(t->child[0]->type == 0){
						if(t->attr.name !=NULL)
					    {symtabError(t, "Void Variable define error");	}
						 break;
					 }
					 if(t->child[0]->type == 1){
							if(st_lookup(scopeST[sp_top], t->attr.name) == NULL){
									t->scope = scopeST[sp_top];
           	st_insert(scopeST[sp_top],t->attr.name,t->child[0]->type,t->lineno,location++);}
							else {symtabError(t, "This variable is redefined in the same scope");	}
						
						break;
				}

				case ArrparamK:
					 if(t->child[0] == NULL) break;
					 if(t->child[0]->type == 0){
						if(t->attr.name !=NULL)
					    {symtabError(t, "Void Variable define error");	}
						 break;
					 }
					 if(t->child[0]->type == 1){
							t->child[0]->type = 3;
							if(st_lookup(scopeST[sp_top], t->attr.name) == NULL){
									t->scope = scopeST[sp_top];
           	st_insert(scopeST[sp_top],t->attr.name,t->child[0]->type,t->lineno,location++);}
							else {symtabError(t, "This variable is redefined in the same scope");	}
						
						break;
				}

				//case CompK:
				//case IfK:
				//case IfelseK:
				//case IterK:
				//case TypeK:
				case VarK:
				case VarrK:
					 if(t->child[0] == NULL) break;
					 if(t->child[0]->type == 0){
						if(t->attr.name !=NULL)
					    {symtabError(t, "Void Variable define error");	}
						 break;
					 }
					 if(t->child[0]->type == 1){
							if(t->child[1] != NULL){
							t->child[0]->type = 3;
							//if(isdigit
							}
							if(st_lookup(scopeST[sp_top], t->attr.name) == NULL){
									t->scope = scopeST[sp_top];
           	st_insert(scopeST[sp_top],t->attr.name,t->child[0]->type,t->lineno,location++);}
							else {symtabError(t, "This variable is redefined in the same scope");	}
						
						break;
				}
					
					
        default:
          break;
      }
      break;
    case ExpK:
      switch (t->kind.exp)
      { 
				//case OpK:
				//case ConstK:
				//case NUMK:
				case IdK:
				case ArrIdK:
					if(t->attr.name != NULL) if(st_lookup(scopeST[sp_top],t->attr.name)!=NULL) {
          st_insert(scopeST[sp_top],t->attr.name,t->type,t->lineno,location++);}
					break;

        default:
          break;
      }
      break;
    default:
      break;
  }
}

static void postNode( TreeNode * t)
{ switch (t->nodekind)
	{ case StmtK:
			switch (t->kind.stmt)
				{ case CompK:
						sc_pop();
						sp_top--;
						
						break;
					default:
						break;
				}
			default:
			break;
	}
}

/* Function buildSymtab constructs the symbol 
 * table by preorder traversal of the syntax tree
 */
void buildSymtab(TreeNode * syntaxTree)
{ sc_insert("global");
	scopeST[sp_top] = "global";	
	io_func_insert();
  traverse(syntaxTree,insertNode,postNode);
	sc_pop();
	sp_top--;
  if (TraceAnalyze && !Error)
  { fprintf(listing,"\nSymbol table:\n\n");
    printSymTab(listing);
  }
}

static void precheckNode(TreeNode * t){
	if(t->nodekind == StmtK && t->kind.stmt == FundecK){
						  scopeST[++sp_top] = t->attr.name;
	}
}

/* Procedure checkNode performs
 * type checking at a single tree node
 */
static void checkNode(TreeNode * t)
{ switch (t->nodekind)
  { case StmtK:
      switch (t->kind.stmt)
      { case IfK:
          if (t->child[0]->type == Integer)
            typeError(t->child[0],"if test is not Boolean");
          break;
        case AssignK:
					scopeName = scopeST[sp_top];
					BucketList left = st_lookup(scopeName, t->child[0]->attr.name);	
					BucketList right = st_lookup(scopeName, t->child[1]->attr.name);	
					if(left->type != right->type) typeError(t,"Assignment two operand types are different");
					if(left->type == 0)
						typeError(left,"void cannot be variable type");
          break;
				case RetK:
					scopeName = scopeST[sp_top];
					if(t->child[0] == NULL) break;
					right = st_lookup(scopeName, t->child[0]->attr.name);
					//should be fixed
					if(right->type != st_lookup("global",scopeName)->type) typeError(t,"Return type is miss match with function");
				  break;	
			  case CompK:
					 sp_top--;
					 break;
					
        default:
          break;
      }
      break;
		case ExpK:
      switch (t->kind.exp)
      { case OpK:
					scopeName = scopeST[sp_top];
					BucketList left = st_lookup(scopeName, t->child[0]->attr.name);
					BucketList right = st_lookup(scopeName, t->child[1]->attr.name);
				  if(left->type != right->type){
						typeError(t,"Operand types are different, invaild expression");
					}
					if(left->type == 3 && right->type == 3){
						typeError(t, "int array type cannot be calculate");
					}
					if(left->type == 0 && right->type == 0){
						typeError(t, "Void type cannot be calculate");
					}
          if ((t->attr.op == EQ) || (t->attr.op == LT))
            t->type = Boolean;
          else
            t->type = Integer;
          break;
        //case ConstK:
        case IdK:
					if(st_lookup(scopeST[sp_top], t->attr.name) == NULL){
						typeError(t,"This variable is not defined");
          }
					break;
				case ArrIdK:
					scopeName = scopeST[sp_top];
					if(t->child[0]->attr.name != NULL){
					BucketList tmp = st_lookup(scopeName, t->child[0]->attr.name);
					if(tmp == NULL){
						typeError(t,"This variable is not defined");
          	}
					else{
						 if(t->child[0]->type != 1) typeError(t,"Array should be accessed by int");
					}}
					break;
        default:
          break;
      }
      break;
    
    default:
      break;

  }
}

/* Procedure typeCheck performs type checking 
 * by a postorder syntax tree traversal
 */
void typeCheck(TreeNode * syntaxTree)
{ 
sp_top = 0;
traverse(syntaxTree,precheckNode,checkNode);
}
