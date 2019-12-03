#ifndef ZENIT_AST_VARIABLE_H
#define ZENIT_AST_VARIABLE_H

#include "node.h"
#include "attribute.h"

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
    struct ZenitAttributeNodeMap attributes;
};

struct ZenitVariableNode* zenit_node_variable_new(struct ZenitSourceLocation location, char *name);
void zenit_node_variable_free(struct ZenitVariableNode *var_node);

#endif /* ZENIT_AST_VARIABLE_H */
