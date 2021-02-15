/****************************************************/
/* File: symtab.c                                   */
/* Symbol table implementation for the TINY compiler*/
/* (allows only one symbol table)                   */
/* Symbol table is implemented as a chained         */
/* hash table                                       */
/* Compiler Construction: Principles and Practice   */
/* Kenneth C. Louden                                */
/****************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "symtab.h"

/* SIZE is the size of the hash table */
#define SIZE 211

/* SHIFT is the power of two used as multiplier
   in hash function  */
#define SHIFT 4

/* the hash function */
static int hash ( char * key )
{ int temp = 0;
  int i = 0;
  while (key[i] != '\0')
  { temp = ((temp << SHIFT) + key[i]) % SIZE;
    ++i;
  }
  return temp;
}

/* the list of line numbers of the source 
 * code in which a variable is referenced
 */


/* The record in the bucket lists for
 * each variable, including name, 
 * assigned memory location, and
 * the list of line numbers in which
 * it appears in the source code
 */
/* the hash table */
static BucketList hashTable[SIZE];
static ScopeList scopehashTable[1000];
static ScopeList scopePT;
static ScopeList scopeST[SIZE];
static int scopeST_ptr = 0;

/* Procedure st_insert inserts line numbers and
 * memory locations into the symbol table
 * loc = memory location is inserted only the
 * first time, otherwise ignored
 */
void sc_insert(char * scope)
{ 
	int sh = hash(scope);
	ScopeList s = scopehashTable[sh];

		//fprintf(listing,"no global scope");
	while((s != NULL) && (strcmp(scope, s->name) != 0))
		s = s->parent;
	//fprintf(listing,"no global scope");
	if (s == NULL)
	{
		s = (ScopeList) malloc(sizeof(struct ScopeListRec));
		s->name = scope;
		s->parent = scopeST[scopeST_ptr]; 
		scopeST[scopeST_ptr++] = s;
		scopehashTable[sh] = s;
		scopePT = s;
	}
	else{
		//already exist this scope 
	}
}

void sc_pop(){
	scopeST_ptr--;	
}

ScopeList sc_find(char * scope){
	
	
	int sh = hash(scope);
	ScopeList s = scopehashTable[sh];
	while((s != NULL) && (strcmp(scope, s->name) != 0))
		s = s->parent;	
		
	return s;
}

ScopeList sc_top(void){
	return scopePT;
}

void st_insert( char * scope, char * name, ExpType type, int lineno, int loc )
{ int h = hash(name);
	// for scope find and allocatio
  //BucketList l =  hashTable[h];
	// fine scope
	scopePT = sc_find(scope);
	if(scopePT == NULL){
		if(name == NULL) fprintf(listing,"NO values");
		//there is no scope defined
		fprintf(listing,"NULL scopePT");
		return;
	}
  BucketList l =  scopePT->bucket[h];

  while ((l != NULL) && (strcmp(name,l->name) != 0))
    l = l->next;
  if (l == NULL) /* variable not yet in table */
  { l = (BucketList) malloc(sizeof(struct BucketListRec));
    l->name = name;
    l->lines = (LineList) malloc(sizeof(struct LineListRec));
    l->lines->lineno = lineno;
    l->memloc = loc;
    l->lines->next = NULL;
		l->type = type;
    l->next = scopePT->bucket[h]; 
    scopePT->bucket[h] = l; }
  else /* found in table, so just add line number */
  { LineList t = l->lines;
    while (t->next != NULL) t = t->next;
    t->next = (LineList) malloc(sizeof(struct LineListRec));
    t->next->lineno = lineno;
    t->next->next = NULL;
  }
} 
/* st_insert */

/* Function st_lookup returns the memory 
 * location of a variable or -1 if not found
 */
BucketList st_lookup ( char * scope, char * name )
{ 
	scopePT = sc_find(scope);
	if(scopePT == NULL){
		//there is no scope defined
		return NULL;
	}

	int h = hash(name);
  BucketList l = scopePT->bucket[h];
  while ((l != NULL) && (strcmp(name,l->name) != 0))
    l = l->next;
  //if (l == NULL) return -1;
  if (l == NULL) {
		return NULL;
	}
  //else return l->memloc;
  else{
	 return l;
	}
}

/* Procedure printSymTab prints a formatted 
 * listing of the symbol table contents 
 * to the listing file
 */
void printSymTab(FILE * listing)
{ int i,j;
  fprintf(listing,"Variable Name   Scope     Type    Location   Line Numbers\n");
  fprintf(listing,"-------------  -------  --------  --------   ------------\n");
  for (i=0;i<SIZE;++i)
  { if (scopehashTable[i] != NULL)
    { 
			ScopeList s = scopehashTable[i];
			for(j=0;j<SIZE;j++){
			BucketList l = s->bucket[j];
      while (l != NULL)
      { LineList t = l->lines;
        fprintf(listing,"%-14s ",l->name);
				fprintf(listing,"%-8s  ",s->name);
				if(l->type == 1) fprintf(listing,"%-9s","Integer");
				else if(l->type == 0) fprintf(listing,"%-9s","Void"); 
				else if(l->type == 3) fprintf(listing,"%-9s","IntArray"); 
        fprintf(listing,"%-8d  ",l->memloc);
        while (t != NULL)
        { fprintf(listing,"%4d ",t->lineno);
          t = t->next;
        }
        fprintf(listing,"\n");
        l = l->next;
      }
			}
    }
  }
} /* printSymTab */