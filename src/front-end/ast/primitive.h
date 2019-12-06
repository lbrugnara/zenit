#ifndef ZENIT_AST_PRIMITIVE_H
#define ZENIT_AST_PRIMITIVE_H

#include "node.h"

union ZenitPrimitiveValue {
    uint8_t uint8;
    uint16_t uint16;
};

/*
 * Struct: struct ZenitPrimitiveNode
 *  Represents a literal value in the program's source.
 * 
 * Members:
 *  <struct ZenitNode> base: Basic information of the node object 
 *  <anonymous union> value: Based on the type information the union is populated with the C value
 * 
 */
struct ZenitPrimitiveNode {
    struct ZenitNode base;
    enum ZenitType type;
    union ZenitPrimitiveValue value;
};

struct ZenitPrimitiveNode* zenit_node_primitive_new(struct ZenitSourceLocation location, enum ZenitType type, union ZenitPrimitiveValue value);
void zenit_node_primitive_free(struct ZenitPrimitiveNode *primitive_node);

#endif /* ZENIT_AST_PRIMITIVE_H */
