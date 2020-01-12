#ifndef ZENIT_AST_TYPE_PRIMITIVE_H
#define ZENIT_AST_TYPE_PRIMITIVE_H

#include "type.h"
#include "../../types/uint.h"

struct ZenitUintTypeNode {
    struct ZenitTypeNode base;
    enum ZenitUintTypeSize size;
};

struct ZenitUintTypeNode* zenit_node_type_uint_new(struct ZenitSourceLocation location, enum ZenitUintTypeSize size);
char* zenit_node_type_uint_uid(struct ZenitUintTypeNode *uint_type_node);
char* zenit_node_type_uint_to_string(struct ZenitUintTypeNode *uint_type_node);
void zenit_node_type_uint_free(struct ZenitUintTypeNode *uint_type_node);

#endif /* ZENIT_AST_TYPE_PRIMITIVE_H */
