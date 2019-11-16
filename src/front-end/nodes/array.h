#ifndef ZENIT_AST_ARRAY_H
#define ZENIT_AST_ARRAY_H

#include "node.h"

/*
 * Struct: struct ZenitArrayInitNode
 *  Represents a literal array initializer
 * 
 * Members:
 *  <struct ZenitNode> base: Basic information of the node object
 *  <struct ZenitNode> **values: Array of pointers to <struct ZenitNode>s that are the individual values
 *              of the array
 */
struct ZenitArrayInitNode {
    struct ZenitNode base;
    struct ZenitNode **elements;
};

#endif /* ZENIT_AST_ARRAY_H */
