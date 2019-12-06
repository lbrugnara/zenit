#ifndef ZENIT_AST_CAST_H
#define ZENIT_AST_CAST_H

#include <stdbool.h>
#include "node.h"
#include "../type.h"

struct ZenitCastNode {
    struct ZenitNode base;    
    struct ZenitNode *expression;
    bool implicit;
};

struct ZenitCastNode* zenit_node_cast_new(struct ZenitSourceLocation location, struct ZenitNode *expression, struct ZenitTypeInfo *to_type, bool implicit);
void zenit_node_cast_free(struct ZenitCastNode *cast_node);

#endif /* ZENIT_AST_CAST_H */
