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
 *  <struct ZenitTypeNode> *type_decl: If present in the declaration, the variable's type
 *  <struct ZenitNode> *rvalue: The right-hand side expression that initializes the variable
 *  <struct ZenitAttributeNodeMap> attributes: If present, a list of all the variable's attributes
 * 
 */
struct ZenitVariableNode {
    struct ZenitNode base;
    char *name;
    struct ZenitTypeNode *type_decl;
    struct ZenitNode *rvalue;
    struct ZenitAttributeNodeMap attributes;
};

struct ZenitVariableNode* zenit_node_variable_new(struct ZenitSourceLocation location, char *name);
char* zenit_node_variable_uid(struct ZenitVariableNode *variable);
char* zenit_node_variable_dump(struct ZenitVariableNode *variable, char *output);
void zenit_node_variable_free(struct ZenitVariableNode *var_node);

#endif /* ZENIT_AST_VARIABLE_H */
