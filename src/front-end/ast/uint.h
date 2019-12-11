#ifndef ZENIT_AST_UINT_H
#define ZENIT_AST_UINT_H

#include "node.h"
#include "../types/uint.h"

union ZenitUintValue {
    uint8_t uint8;
    uint16_t uint16;
};

struct ZenitUintNode {
    struct ZenitNode base;
    enum ZenitUintTypeSize size;
    union ZenitUintValue value;
};

struct ZenitUintNode* zenit_node_uint_new(struct ZenitSourceLocation location, enum ZenitUintTypeSize size, union ZenitUintValue value);
char* zenit_node_uint_uid(struct ZenitUintNode *uint_node);
void zenit_node_uint_free(struct ZenitUintNode *uint_node);

#endif /* ZENIT_AST_UINT_H */
