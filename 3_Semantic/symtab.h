/****************************************************/
/* File: symtab.h                                   */
/* Symbol table interface for the TINY compiler     */
/* (allows only one symbol table)                   */
/* Compiler Construction: Principles and Practice   */
/* Kenneth C. Louden                                */
/****************************************************/

#ifndef _SYMTAB_H_
#define _SYMTAB_H_

#define SIZE 211

#include "globals.h"

/* Procedure st_insert inserts line numbers and
 * memory locations into the symbol table
 * loc = memory location is inserted only the
 * first time, otherwise ignored
 */

typedef struct LineListRec *LineList;
typedef struct BucketListRec * BucketList;
typedef struct ScopeListRec * ScopeList;
typedef struct LineListRec
   { int lineno;
     struct LineListRec * next;
   } * LineList;

typedef struct BucketListRec
	{ char * name;
		ExpType type;
		LineList lines;
		int memloc;
		struct BucketListRec * next;
	} * BucketList;

typedef struct ScopeListRec
	{ char * name;
		BucketList bucket[SIZE];
		struct ScopeListRec * parent;
	} * ScopeList;

void st_insert( char * scope, char * name, ExpType type, int lineno, int loc );

/* Function st_lookup returns the memory 
 * location of a variable or -1 if not found
 */
//int st_lookup ( char * name );
BucketList st_lookup( char * scope, char * name );
BucketList st_lookup_excluding_parent( char * scope, char * name );

/* Procedure printSymTab prints a formatted 
 * listing of the symbol table contents 
 * to the listing file
 */
void printSymTab(FILE * listing);

#endif
