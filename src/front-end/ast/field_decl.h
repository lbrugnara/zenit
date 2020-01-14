#ifndef ZENIT_AST_FIELD_DECL_H
#define ZENIT_AST_FIELD_DECL_H

#include "node.h"
#include "attribute.h"

/*
 * Struct: struct ZenitFieldDeclNode
 *  Represents a field declaration node
 * 
 * Members:
 *  <struct ZenitNode> base: Basic information of the node object
 *  <char> *name: The field name
 *  <struct ZenitTypeNode> *type_decl: The field's type information
 * 
 */
struct ZenitFieldDeclNode {
    struct ZenitNode base;
    char *name;
    struct ZenitTypeNode *type_decl;
    struct ZenitNode *owner;
};

struct ZenitFieldDeclNode* zenit_node_field_decl_new(struct ZenitSourceLocation location, char *name);
char* zenit_node_field_decl_uid(struct ZenitFieldDeclNode *field_decl);
char* zenit_node_field_decl_dump(struct ZenitFieldDeclNode *field_decl, char *output);
void zenit_node_field_decl_free(struct ZenitFieldDeclNode *field_decl);

#endif /* ZENIT_AST_FIELD_DECL_H */
