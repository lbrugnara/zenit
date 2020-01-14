#ifndef ZENIT_AST_NODE_H
#define ZENIT_AST_NODE_H

#include "../token.h"
#include "../types/type.h"

/*
 * Enum: enum ZenitNodeType 
 *  Types of AST node
 */
enum ZenitNodeType {
    ZENIT_NODE_UINT,
    ZENIT_NODE_VARIABLE,
    ZENIT_NODE_STRUCT_DECL,
    ZENIT_NODE_FIELD_DECL,
    ZENIT_NODE_ATTRIBUTE,
    ZENIT_NODE_PROPERTY,
    ZENIT_NODE_ARRAY,
    ZENIT_NODE_IDENTIFIER,
    ZENIT_NODE_REFERENCE,
    ZENIT_NODE_CAST,
    ZENIT_NODE_STRUCT,
    ZENIT_NODE_FIELD,
    ZENIT_NODE_TYPE_ARRAY,
    ZENIT_NODE_TYPE_UINT,
    ZENIT_NODE_TYPE_REFERENCE,
    ZENIT_NODE_TYPE_STRUCT,
};

/*
 * Struct: struct ZenitNode
 *  The base node object. All the specific objects are compound with this one
 * 
 * Members:
 *  <enum ZenitNodeType> type: The specific type of the AST node
 *  <struct ZenitSourceLocation> location: The place in the source code represented by the node
 */
struct ZenitNode {
    enum ZenitNodeType type;
    struct ZenitSourceLocation location;
};

/*
 * Function: zenit_node_uid
 *  Returns a UID for the node object
 *
 * Parameters:
 *  <struct ZenitNode> *node: Node object
 *
 * Returns:
 *  <char>*: UID of the node object
 *
 */
char* zenit_node_uid(struct ZenitNode *node);

/*
 * Function: zenit_node_to_string
 *  Returns a string representation of the node's type
 *
 * Parameters:
 *  <struct ZenitNode> *node: Node object
 *
 * Returns:
 *  <char>*: String representation of the node's type
 * 
 */
char* zenit_node_to_string(struct ZenitNode *node);

char* zenit_node_dump(struct ZenitNode *node, char *output);

/*
 * Function: zenit_node_free
 *  This function releases the memory allocated in the *node* object
 *  independently of the type of node
 *
 * Parameters:
 *  <struct ZenitNode> *node: Node object to free
 *
 * Returns:
 *  <void>: This function does not return a value
 * 
 * Notes:
 *  All the objects compound with a <struct ZenitNode> object can be
 *  freed using this function
 */
void zenit_node_free(struct ZenitNode *node);

#endif /* ZENIT_AST_NODE_H */
