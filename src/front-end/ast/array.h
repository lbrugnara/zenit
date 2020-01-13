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
 *  <struct ZenitNode> **elements: Pointers to the array's node elements
 */
struct ZenitArrayNode {
    struct ZenitNode base;
    struct ZenitNode **elements;
};

struct ZenitArrayNode* zenit_node_array_new(struct ZenitSourceLocation location);
char* zenit_node_array_uid(struct ZenitArrayNode *array);
void zenit_node_array_add_child(struct ZenitArrayNode *array, struct ZenitNode *element);
char* zenit_node_array_dump(struct ZenitArrayNode *array, char *output);
void zenit_node_array_free(struct ZenitArrayNode *array);

#endif /* ZENIT_AST_ARRAY_H */
