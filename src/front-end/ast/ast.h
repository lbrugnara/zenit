#ifndef ZENIT_NODE_H
#define ZENIT_NODE_H

#include <stdint.h>
#include "../token.h"
#include "../types/type.h"

#include "array.h"
#include "bool.h"
#include "attribute.h"
#include "cast.h"
#include "field.h"
#include "field_decl.h"
#include "identifier.h"
#include "node.h"
#include "property.h"
#include "reference.h"
#include "struct.h"
#include "struct_decl.h"
#include "types/type.h"
#include "uint.h"
#include "variable.h"

/*
 * Struct: struct ZenitAst
 *  Represents the abstract syntax tree of the parsed program
 * 
 * Members:
 *  <struct ZenitNode> **decls: Array of pointers that contains the declarations of the program
 * 
 */
struct ZenitAst {
    struct ZenitNode **decls;
};

/*
 * Function: zenit_ast_new
 *  Creates a new AST object
 *
 * Parameters:
 *  <struct ZenitNode> **decls: Array of declarations to be included in the AST
 *
 * Returns:
 *  struct ZenitAst*: AST object
 *
 * Notes:
 *  The AST object takes ownership of the *decls* array, which means that the caller does not need to free
 *  the memory used by the declaration objects 
 */
struct ZenitAst* zenit_ast_new(struct ZenitNode **decls);

/*
 * Function: zenit_ast_dump
 *  Returns a heap allocated string containing a dump of the AST object
 *
 * Parameters:
 *  <struct ZenitAst> *ast: AST object to dump to the output
 *
 * Returns:
 *  char*: Pointer to a heap allocated string containing the dump of the AST
 *
 * Notes:
 *  The string returned by this function must be freed with the <fl_cstring_free> function
 */
char* zenit_ast_dump(struct ZenitAst *ast);

/*
 * Function: zenit_ast_free
 *  Releases the memory of a whole <struct ZenitAst> object
 *
 * Parameters:
 *  <struct ZenitAst> *ast - AST object to free
 *
 * Returns:
 *  <void>: This function does not return a value
 */
void zenit_ast_free(struct ZenitAst *ast);

#endif /* ZENIT_NODE_H */
