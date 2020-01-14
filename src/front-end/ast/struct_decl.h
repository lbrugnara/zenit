#ifndef ZENIT_AST_STRUCT_DECL_H
#define ZENIT_AST_STRUCT_DECL_H

#include "node.h"
#include "attribute.h"

/*
 * Struct: struct ZenitStructDeclNode
 *  Represents a struct declaration node
 * 
 * Members:
 *  <struct ZenitNode> base: Basic information of the node object
 *  <char> *name: The struct name
 *  <struct ZenitNode> **members: Pointers to the struct members
 *  <struct ZenitAttributeNodeMap> attributes: If present, a list of all the struct attributes
 * 
 */
struct ZenitStructDeclNode {
    struct ZenitNode base;
    char *name;
    struct ZenitNode **members;
    struct ZenitAttributeNodeMap attributes;
};

struct ZenitStructDeclNode* zenit_node_struct_decl_new(struct ZenitSourceLocation location, char *name);
char* zenit_node_struct_decl_uid(struct ZenitStructDeclNode *struct_node);
char* zenit_node_struct_decl_dump(struct ZenitStructDeclNode *struct_node, char *output);
void zenit_node_struct_decl_free(struct ZenitStructDeclNode *struct_node);

#endif /* ZENIT_AST_STRUCT_DECL_H */
