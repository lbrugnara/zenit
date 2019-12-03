#ifndef ZENIT_AST_LITERAL_H
#define ZENIT_AST_LITERAL_H

#include "node.h"

union ZenitLiteralValue {
    uint8_t uint8;
    uint16_t uint16;
};

/*
 * Struct: struct ZenitLiteralNode
 *  Represents a literal value in the program's source.
 * 
 * Members:
 *  <struct ZenitNode> base: Basic information of the node object 
 *  <anonymous union> value: Based on the type information the union is populated with the C value
 * 
 */
struct ZenitLiteralNode {
    struct ZenitNode base;    
    union ZenitLiteralValue value;
};

struct ZenitLiteralNode* zenit_node_literal_new(struct ZenitSourceLocation location, enum ZenitType type, union ZenitLiteralValue value);
void zenit_node_literal_free(struct ZenitLiteralNode *literal_node);

#endif /* ZENIT_AST_LITERAL_H */
