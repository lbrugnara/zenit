#ifndef ZENIT_NODE_H
#define ZENIT_NODE_H

#include <stdint.h>
#include "../token.h"
#include "../types/type.h"

#include "array.h"
#include "attribute.h"
#include "cast.h"
#include "field_decl.h"
#include "identifier.h"
#include "node.h"
#include "property.h"
#include "reference.h"
#include "struct_decl.h"
#include "types/type.h"
#include "uint.h"
#include "variable.h"

/*
 * Struct: struct ZenitAst
 *  Represents the abstract syntax tree of the parsed program
 * 
 * Members:
 *  <struct ZenitNode> **decls: -fllib- array of pointers that contains the declarations of the program
 * 
 */
struct ZenitAst {
    struct ZenitNode **decls;
};

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
