#ifndef CENIT_NODE_H
#define CENIT_NODE_H

#include <stdint.h>
#include "token.h"
#include "type.h"

/*
 * Enum: CenitNodeType 
 *  Types of AST node
 */
typedef enum CenitNodeType {
    CENIT_NODE_LITERAL,
    CENIT_NODE_VARIABLE,
    CENIT_NODE_ARRAY_INIT,
} CenitNodeType;

/*
 * Struct: CenitNode
 *  The base node object. All the specific objects are compound with this one
 */
typedef struct CenitNode {
    CenitNodeType type;
    CenitSourceLocation location;
} CenitNode;

/*
 * Struct: CenitLiteralNode
 *  Represents a literal value in the program's source.
 * 
 * Members:
 *  <CenitNode> base: Basic information of the node object
 *  <CenitTypeInfo> typeinfo: Contains the literal's type information
 *  <anonymous union> value: Based on the type information the union is populated with the C value
 * 
 */
typedef struct CenitLiteralNode {
    CenitNode base;
    CenitTypeInfo typeinfo;
    union {
        uint8_t uint8;
    } value;
} CenitLiteralNode;

/*
 * Struct: CenitVariableNode
 *  Represents a variable declaration node
 * 
 * Members:
 *  <CenitNode> base: Basic information of the node object
 *  <CenitTypeInfo> typeinfo: Contains the variable declaration type information, if present (can be inferred)
 *  <char> *name: The variable name
 *  <CenitNode> *value: The right-hand side expression that initializes the variable
 */
typedef struct CenitVariableNode {
    CenitNode base;
    CenitTypeInfo typeinfo;
    char *name;
    CenitNode *value;
} CenitVariableNode;

/*
 * Struct: CenitArrayInitNode
 *  Represents a literal array initializer
 * 
 * Members:
 *  <CenitNode> base: Basic information of the node object
 *  <CenitNode> **values: Array of pointers to <CenitNode>s that are the individual values
 *              of the array
 */
typedef struct CenitArrayInitNode {
    CenitNode base;
    CenitNode **values;
    CenitTypeInfo typeinfo;
} CenitArrayInitNode;

/*
 * Struct: CenitAst
 *  Represents the abstract syntax tree of the parsed program
 * 
 * Members:
 *  <CenitNode> **decls: -fllib- array of pointers that contains the declarations of the program
 * 
 */
typedef struct CenitAst {
    CenitNode **decls;
} CenitAst;

/*
 * Function: cenit_node_free
 *  This function releases the memory allocated in the *node* object
 *  independently of the type of node
 *
 * Parameters:
 *  node - Node object to free
 *
 * Returns:
 *  void - This function does not return a value
 */
void cenit_node_free(CenitNode *node);

/*
 * Function: cenit_node_array_free
 *  Releases the memory of an array of <CenitNode> pointers.
 *  The array must be an array allocated with the fllib's 
 *  <fl_array_new> function
 *
 * Parameters:
 *  array - An <FlArray> object
 *
 * Returns:
 *  void - This function does not return a value
 * 
 * Notes:
 *  If *array* is NULL, this function returns immediately
 *  without performing anything nor erroring out
 */
void cenit_node_array_free(CenitNode **array);

/*
 * Function: cenit_ast_free
 *  Releases the memory of a whole <CenitAst> object
 *
 * Parameters:
 *  ast - AST object to free
 *
 * Returns:
 *  void - This function does not return a value
 */
void cenit_ast_free(CenitAst *ast);

#endif /* CENIT_NODE_H */
