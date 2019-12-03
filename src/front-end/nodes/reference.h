#ifndef ZENIT_AST_REFERENCE_H
#define ZENIT_AST_REFERENCE_H

#include "node.h"

/*
 * Struct: struct ZenitUnaryRefNode
 *  Represents taking an expression address (reference operator)
 * 
 * Members:
 *  <struct ZenitNode> base: Basic information of the node object
 *  <struct ZenitNode> *expression: The expression to take its address from
 */
struct ZenitUnaryRefNode {
    struct ZenitNode base;
    struct ZenitNode *expression;
};

struct ZenitUnaryRefNode* zenit_node_unary_ref_new(struct ZenitSourceLocation location, struct ZenitNode *expression);
void zenit_node_unary_ref_free(struct ZenitUnaryRefNode *ref_node);

#endif /* ZENIT_AST_REFERENCE_H */
