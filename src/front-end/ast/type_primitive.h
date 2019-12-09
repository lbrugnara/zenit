#ifndef ZENIT_AST_TYPE_PRIMITIVE_H
#define ZENIT_AST_TYPE_PRIMITIVE_H

#include "type.h"

struct ZenitPrimitiveTypeNode {
    struct ZenitTypeNode base;
};

struct ZenitPrimitiveTypeNode* zenit_node_type_primitive_new(struct ZenitSourceLocation location, enum ZenitType type);
char* zenit_node_type_primitive_uid(struct ZenitPrimitiveTypeNode *type_node);
void zenit_node_type_primitive_free(struct ZenitPrimitiveTypeNode *type_node);

#endif /* ZENIT_AST_TYPE_PRIMITIVE_H */
