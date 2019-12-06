#include <fllib.h>
#include "cast.h"

struct ZenitCastNode* zenit_node_cast_new(struct ZenitSourceLocation location, struct ZenitNode *expression, bool implicit)
{
    struct ZenitCastNode *cast_node = fl_malloc(sizeof(struct ZenitCastNode));
    cast_node->base.type = ZENIT_NODE_CAST;
    cast_node->base.location = location;
    cast_node->implicit = implicit;
    cast_node->expression = expression;

    return cast_node;
}

void zenit_node_cast_free(struct ZenitCastNode *cast_node)
{
    if (!cast_node)
        return;

    if (cast_node->expression)
        zenit_node_free(cast_node->expression);

    if (cast_node->type_decl)
        zenit_node_free((struct ZenitNode*) cast_node->type_decl);

    fl_free(cast_node);
}
