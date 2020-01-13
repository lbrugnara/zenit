#ifndef ZENIT_AST_STRUCT_H
#define ZENIT_AST_STRUCT_H

#include "node.h"
#include "attribute.h"

/*
 * Struct: struct ZenitStructNode
 *  Represents a struct declaration node
 * 
 * Members:
 *  <struct ZenitNode> base: Basic information of the node object
 *  <char> *name: The struct name
 *  <struct ZenitNode> **members: Pointers to the struct members
 *  <struct ZenitAttributeNodeMap> attributes: If present, a list of all the struct attributes
 * 
 */
struct ZenitStructNode {
    struct ZenitNode base;
    char *name;
    struct ZenitNode **members;
    struct ZenitAttributeNodeMap attributes;
};

struct ZenitStructNode* zenit_node_struct_new(struct ZenitSourceLocation location, char *name);
char* zenit_node_struct_uid(struct ZenitStructNode *struct_node);
char* zenit_node_struct_dump(struct ZenitStructNode *struct_node, char *output);
void zenit_node_struct_free(struct ZenitStructNode *struct_node);

#endif /* ZENIT_AST_STRUCT_H */
