#ifndef ZENIT_AST_TYPE_ARRAY_H
#define ZENIT_AST_TYPE_ARRAY_H

#include "type.h"

struct ZenitArrayTypeNode {
    struct ZenitTypeNode base;
    struct ZenitTypeNode *members_type;
    size_t length;
};

struct ZenitArrayTypeNode* zenit_node_type_array_new(struct ZenitSourceLocation location, struct ZenitTypeNode *members_type, size_t length);
void zenit_node_type_array_free(struct ZenitArrayTypeNode *type_node);

#endif /* ZENIT_AST_TYPE_ARRAY_H */
