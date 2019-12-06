#ifndef ZENIT_AST_TYPE_REFERENCE_H
#define ZENIT_AST_TYPE_REFERENCE_H

#include "type.h"

struct ZenitReferenceTypeNode {
    struct ZenitTypeNode base;
    struct ZenitTypeNode *element;
};

struct ZenitReferenceTypeNode* zenit_node_type_reference_new(struct ZenitSourceLocation location, struct ZenitTypeNode *type);
void zenit_node_type_reference_free(struct ZenitReferenceTypeNode *type_node);

#endif /* ZENIT_AST_TYPE_REFERENCE_H */
