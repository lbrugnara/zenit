#ifndef ZENIT_AST_NODE_H
#define ZENIT_AST_NODE_H

#include "../token.h"
#include "../types/type.h"

/*
 * Enum: ZenitNodeKind 
 *  Types of AST node
 */
typedef enum ZenitNodeKind {
    ZENIT_AST_NODE_UINT,
    ZENIT_AST_NODE_BOOL,
    ZENIT_AST_NODE_IF,
    ZENIT_AST_NODE_BLOCK,
    ZENIT_AST_NODE_VARIABLE,
    ZENIT_AST_NODE_STRUCT_DECL,
    ZENIT_AST_NODE_FIELD_DECL,
    ZENIT_AST_NODE_ATTRIBUTE,
    ZENIT_AST_NODE_PROPERTY,
    ZENIT_AST_NODE_ARRAY,
    ZENIT_AST_NODE_IDENTIFIER,
    ZENIT_AST_NODE_REFERENCE,
    ZENIT_AST_NODE_CAST,
    ZENIT_AST_NODE_STRUCT,
    ZENIT_AST_NODE_FIELD,
    ZENIT_AST_NODE_TYPE_UINT,
    ZENIT_AST_NODE_TYPE_BOOL,
    ZENIT_AST_NODE_TYPE_ARRAY,
    ZENIT_AST_NODE_TYPE_REFERENCE,
    ZENIT_AST_NODE_TYPE_STRUCT,
} ZenitNodeKind;

/*
 * Struct: ZenitNode
 *  The base node object. All the specific objects are compound with this one
 * 
 * Members:
 *  <ZenitNodeKind> type: The specific kind of AST node
 *  <ZenitSourceLocation> location: The place in the source code represented by the node
 */
typedef struct ZenitNode {
    ZenitNodeKind nodekind;
    ZenitSourceLocation location;
} ZenitNode;

/*
 * Function: zenit_node_uid
 *  Returns a UID for the node object
 *
 * Parameters:
 *  <ZenitNode> *node: Node object
 *
 * Returns:
 *  <char>*: UID of the node object
 *
 * Notes:
 *  The object returned by this function must be freed using the
 *  <fl_cstring_free> function.
 */
char* zenit_node_uid(ZenitNode *node);

/*
 * Function: zenit_node_dump
 *  Appends a dump of the node object to the output pointer, and
 *  returns a pointer to the -possibly reallocated- output
 *
 * Parameters:
 *  <ZenitNode> *node: Node object to dump to the output
 *  <char> *output: Pointer to a heap allocated string
 *
 * Returns:
 *  char*: Pointer to the output string
 *
 * Notes:
 *  Because the *output* pointer can be modified, this function returns
 *  a pointer to the new location in case the memory is reallocated or
 *  to the old location in case the pointer does not need to be modified. Either
 *  way, it is safe to use the function as:
 *      output = zenit_node_dump(node, output);
 *  If the memory of *output* cannot be reallocated this function frees the memory.
 */
char* zenit_node_dump(ZenitNode *node, char *output);

/*
 * Function: zenit_node_free
 *  This function releases the memory allocated in the *node* object
 *  independently of the type of node
 *
 * Parameters:
 *  <ZenitNode> *node: Node object to free
 *
 * Returns:
 *  <void>: This function does not return a value
 * 
 * Notes:
 *  All the objects compound with a <ZenitNode> object can be
 *  freed using this function
 */
void zenit_node_free(ZenitNode *node);

#endif /* ZENIT_AST_NODE_H */
