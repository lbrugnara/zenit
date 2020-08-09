#ifndef ZENIT_NODE_H
#define ZENIT_NODE_H

#include <stdint.h>
#include "../token.h"
#include "../types/type.h"

#include "array.h"
#include "block.h"
#include "bool.h"
#include "attribute.h"
#include "cast.h"
#include "field.h"
#include "field-decl.h"
#include "identifier.h"
#include "if.h"
#include "node.h"
#include "property.h"
#include "reference.h"
#include "struct.h"
#include "struct-decl.h"
#include "types/type.h"
#include "uint.h"
#include "variable.h"

/*
 * Struct: ZenitAst
 *  Represents the abstract syntax tree of the parsed program
 * 
 * Members:
 *  <ZenitNode> **decls: Array of pointers that contains the declarations of the program
 * 
 */
typedef struct ZenitAst {
    ZenitNode **decls;
} ZenitAst;

/*
 * Function: zenit_ast_new
 *  Creates a new AST object
 *
 * Parameters:
 *  <ZenitNode> **decls: Array of declarations to be included in the AST
 *
 * Returns:
 *  ZenitAst*: AST object
 *
 * Notes:
 *  The AST object takes ownership of the *decls* array, which means that the caller does not need to free
 *  the memory used by the declaration objects 
 */
ZenitAst* zenit_ast_new(ZenitNode **decls);

/*
 * Function: zenit_ast_dump
 *  Returns a heap allocated string containing a dump of the AST object
 *
 * Parameters:
 *  <ZenitAst> *ast: AST object to dump to the output
 *
 * Returns:
 *  char*: Pointer to a heap allocated string containing the dump of the AST
 *
 * Notes:
 *  The string returned by this function must be freed with the <fl_cstring_free> function
 */
char* zenit_ast_dump(ZenitAst *ast);

/*
 * Function: zenit_ast_free
 *  Releases the memory of a whole <ZenitAst> object
 *
 * Parameters:
 *  <ZenitAst> *ast - AST object to free
 *
 * Returns:
 *  <void>: This function does not return a value
 */
void zenit_ast_free(ZenitAst *ast);

#endif /* ZENIT_NODE_H */
