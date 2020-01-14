#ifndef ZENIT_AST_STRUCT_H
#define ZENIT_AST_STRUCT_H

#include "node.h"

/*
 * Struct: struct ZenitStructNode
 *  Represents a struct node
 * 
 * Members:
 *  <struct ZenitNode> base: Basic information of the node object
 *  <char> *name: The struct name
 *  <struct ZenitNode> **members: Pointers to the struct members
 * 
 */
struct ZenitStructNode {
    struct ZenitNode base;
    char *name;
    struct ZenitNode **members;
};

struct ZenitStructNode* zenit_node_struct_new(struct ZenitSourceLocation location, char *name);
char* zenit_node_struct_uid(struct ZenitStructNode *struct_node);
char* zenit_node_struct_dump(struct ZenitStructNode *struct_node, char *output);
void zenit_node_struct_free(struct ZenitStructNode *struct_node);

#endif /* ZENIT_AST_STRUCT_H */
