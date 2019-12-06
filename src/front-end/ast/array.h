#ifndef ZENIT_AST_ARRAY_H
#define ZENIT_AST_ARRAY_H

#include "node.h"
#include "../types/array.h"

/*
 * Struct: struct ZenitArrayNode
 *  Represents a literal array initializer
 * 
 * Members:
 *  <struct ZenitNode> base: Basic information of the node object
 *  <struct ZenitNode> **values: Array of pointers to <struct ZenitNode>s that are the individual values
 *              of the array
 */
struct ZenitArrayNode {
    struct ZenitNode base;
    struct ZenitTypeInfo *typeinfo;
    struct ZenitNode **elements;
};

struct ZenitArrayNode* zenit_node_array_new(struct ZenitSourceLocation location);
void zenit_node_array_add_child(struct ZenitArrayNode *array, struct ZenitNode *element);
void zenit_node_array_free(struct ZenitArrayNode *array);

#endif /* ZENIT_AST_ARRAY_H */
