#ifndef ZENIT_AST_FIELD_H
#define ZENIT_AST_FIELD_H

#include "node.h"

/*
 * Struct: struct ZenitFieldNode
 *  Represents a field node
 * 
 * Members:
 *  <struct ZenitNode> base: Basic information of the node object
 *  <char> *name: The field name
 *  <struct ZenitNode> *value: The field's value
 *  <struct ZenitNode> *owner: The field's parent node
 * 
 */
struct ZenitFieldNode {
    struct ZenitNode base;
    char *name;
    struct ZenitNode *value;
    struct ZenitNode *owner;
};

struct ZenitFieldNode* zenit_node_field_new(struct ZenitSourceLocation location, char *name);
char* zenit_node_field_uid(struct ZenitFieldNode *field);
char* zenit_node_field_dump(struct ZenitFieldNode *field, char *output);
void zenit_node_field_free(struct ZenitFieldNode *field);

#endif /* ZENIT_AST_FIELD_H */
