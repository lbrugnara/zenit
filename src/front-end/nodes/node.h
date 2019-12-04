#ifndef ZENIT_AST_NODE_H
#define ZENIT_AST_NODE_H

#include "../token.h"
#include "../type.h"

/*
 * Enum: enum ZenitNodeType 
 *  Types of AST node
 */
enum ZenitNodeType {
    ZENIT_NODE_LITERAL,
    ZENIT_NODE_VARIABLE,
    ZENIT_NODE_ATTRIBUTE,
    ZENIT_NODE_ATTRIBUTE_PROPERTY,
    ZENIT_NODE_ARRAY_INIT,
    ZENIT_NODE_IDENTIFIER,
    ZENIT_NODE_UNARY_REF,
    ZENIT_NODE_CAST,
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

#endif /* ZENIT_AST_NODE_H */
