
#include <fllib/Cstring.h>
#include "cast.h"

struct ZenitCastNode* zenit_node_cast_new(struct ZenitSourceLocation location, struct ZenitNode *expression, bool implicit)
{
    struct ZenitCastNode *cast_node = fl_malloc(sizeof(struct ZenitCastNode));
    cast_node->base.nodekind = ZENIT_NODE_CAST;
    cast_node->base.location = location;
    cast_node->implicit = implicit;
    cast_node->expression = expression;

    return cast_node;
}

char* zenit_node_cast_uid(struct ZenitCastNode *cast)
{
    if (!cast)
        return NULL;

    if (cast->implicit)
        return fl_cstring_vdup("%%L%u:C%u_cast_impl", cast->base.location.line, cast->base.location.col);
    return fl_cstring_vdup("%%L%u:C%u_cast", cast->base.location.line, cast->base.location.col);
}

char* zenit_node_cast_dump(struct ZenitCastNode *cast, char *output)
{
    fl_cstring_append(&output, "(cast ");

    output = zenit_node_dump(cast->expression, output);

    if (cast->type_decl != NULL)
        output = zenit_node_dump((struct ZenitNode*) cast->type_decl, output);

    fl_cstring_append(&output, ")");

    return output;
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
