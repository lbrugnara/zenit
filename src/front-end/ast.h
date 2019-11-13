#ifndef ZENIT_NODE_H
#define ZENIT_NODE_H

#include <stdint.h>
#include "token.h"
#include "type.h"

/*
 * Enum: enum ZenitNodeType 
 *  Types of AST node
 */
enum ZenitNodeType {
    ZENIT_NODE_LITERAL,
    ZENIT_NODE_VARIABLE,
    ZENIT_NODE_ATTRIBUTE,
    ZENIT_NODE_ARRAY_INIT,
    ZENIT_NODE_IDENTIFIER,
    ZENIT_NODE_UNARY_REF,
};

/*
 * Struct: struct ZenitNode
 *  The base node object. All the specific objects are compound with this one
 * 
 * Members:
 *  <enum ZenitNodeType> type: The specific type of the AST node
 *  <struct ZenitSourceLocation> location: The place in the source code represented by the node
 *  <struct ZenitTypeInfo> typeinfo: The type of the operand or operator represented by the node
 */
struct ZenitNode {
    enum ZenitNodeType type;
    struct ZenitSourceLocation location;
    struct ZenitTypeInfo typeinfo;
};

/*
 * Struct: struct ZenitLiteralNode
 *  Represents a literal value in the program's source.
 * 
 * Members:
 *  <struct ZenitNode> base: Basic information of the node object 
 *  <anonymous union> value: Based on the type information the union is populated with the C value
 * 
 */
struct ZenitLiteralNode {
    struct ZenitNode base;    
    union {
        uint8_t uint8;
        uint16_t uint16;
    } value;
};

/*
 * Struct: struct ZenitVariableNode
 *  Represents a variable declaration node
 * 
 * Members:
 *  <struct ZenitNode> base: Basic information of the node object
 *  <char> *name: The variable name
 *  <struct ZenitNode> *value: The right-hand side expression that initializes the variable
 */
struct ZenitVariableNode {
    struct ZenitNode base;
    char *name;
    struct ZenitNode *value;
    struct ZenitAttributeNode **attributes;
};

/*
 * Struct: struct ZenitArrayInitNode
 *  Represents a literal array initializer
 * 
 * Members:
 *  <struct ZenitNode> base: Basic information of the node object
 *  <struct ZenitNode> **values: Array of pointers to <struct ZenitNode>s that are the individual values
 *              of the array
 */
struct ZenitArrayInitNode {
    struct ZenitNode base;
    struct ZenitNode **elements;
};

/*
 * Struct: struct ZenitUnaryRefNode
 *  Represents taking an expression address (reference operator)
 * 
 * Members:
 *  <struct ZenitNode> base: Basic information of the node object
 *  <struct ZenitNode> *expression: The expression to take its address from
 */
struct ZenitUnaryRefNode {
    struct ZenitNode base;
    struct ZenitNode *expression;
};

/*
 * Struct: struct ZenitIdentifierNode
 *  Represents a reference to an identifier
 */
struct ZenitIdentifierNode {
    struct ZenitNode base;
    char *name;
};

struct ZenitAttributePropertyNode {
    char *name;
    struct ZenitNode *value;
};

struct ZenitAttributeNode {
    struct ZenitNode base;
    char *name;
    struct ZenitAttributePropertyNode *properties;
};

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

/*
 * Function: zenit_node_free
 *  This function releases the memory allocated in the *node* object
 *  independently of the type of node
 *
 * Parameters:
 *  node - Node object to free
 *
 * Returns:
 *  void - This function does not return a value
 */
void zenit_node_free(struct ZenitNode *node);

/*
 * Function: zenit_node_array_free
 *  Releases the memory of an array of <struct ZenitNode> pointers.
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
void zenit_node_array_free(struct ZenitNode **array);

/*
 * Function: zenit_ast_free
 *  Releases the memory of a whole <struct ZenitAst> object
 *
 * Parameters:
 *  ast - AST object to free
 *
 * Returns:
 *  void - This function does not return a value
 */
void zenit_ast_free(struct ZenitAst *ast);

#endif /* ZENIT_NODE_H */
