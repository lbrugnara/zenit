#ifndef ZENIT_AST_REFERENCE_H
#define ZENIT_AST_REFERENCE_H

#include "node.h"

/*
 * Struct: struct ZenitReferenceNode
 *  Represents taking an expression address (reference operator)
 * 
 * Members:
 *  <struct ZenitNode> base: Basic information of the node object
 *  <struct ZenitNode> *expression: The expression to take its address from
 */
struct ZenitReferenceNode {
    struct ZenitNode base;
    struct ZenitNode *expression;
};

struct ZenitReferenceNode* zenit_node_reference_new(struct ZenitSourceLocation location, struct ZenitNode *expression);
void zenit_node_reference_free(struct ZenitReferenceNode *ref_node);

#endif /* ZENIT_AST_REFERENCE_H */
