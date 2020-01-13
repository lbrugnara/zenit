#ifndef ZENIT_AST_FIELD_H
#define ZENIT_AST_FIELD_H

#include "node.h"
#include "attribute.h"

/*
 * Struct: struct ZenitFieldNode
 *  Represents a field declaration node
 * 
 * Members:
 *  <struct ZenitNode> base: Basic information of the node object
 *  <char> *name: The field name
 *  <struct ZenitTypeNode> *type_decl: The field's type information
 * 
 */
struct ZenitFieldNode {
    struct ZenitNode base;
    char *name;
    struct ZenitTypeNode *type_decl;
    struct ZenitNode *owner;
};

struct ZenitFieldNode* zenit_node_field_new(struct ZenitSourceLocation location, char *name);
char* zenit_node_field_uid(struct ZenitFieldNode *field_node);
char* zenit_node_field_dump(struct ZenitFieldNode *field, char *output);
void zenit_node_field_free(struct ZenitFieldNode *field_node);

#endif /* ZENIT_AST_FIELD_H */
