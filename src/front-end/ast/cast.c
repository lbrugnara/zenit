
#include <fllib/Cstring.h>
#include "cast.h"

ZenitCastNode* zenit_cast_node_new(ZenitSourceLocation location, ZenitNode *expression, bool implicit)
{
    ZenitCastNode *cast_node = fl_malloc(sizeof(ZenitCastNode));
    cast_node->base.nodekind = ZENIT_NODE_CAST;
    cast_node->base.location = location;
    cast_node->implicit = implicit;
    cast_node->expression = expression;

    return cast_node;
}

char* zenit_cast_node_uid(ZenitCastNode *cast)
{
    if (!cast)
        return NULL;

    if (cast->implicit)
        return fl_cstring_vdup("%%L%u:C%u_cast_impl", cast->base.location.line, cast->base.location.col);
    return fl_cstring_vdup("%%L%u:C%u_cast", cast->base.location.line, cast->base.location.col);
}

char* zenit_cast_node_dump(ZenitCastNode *cast, char *output)
{
    fl_cstring_append(&output, "(cast ");

    output = zenit_node_dump(cast->expression, output);

    if (cast->type_decl != NULL)
        output = zenit_node_dump((ZenitNode*) cast->type_decl, output);

    fl_cstring_append(&output, ")");

    return output;
}

void zenit_cast_node_free(ZenitCastNode *cast_node)
{
    if (!cast_node)
        return;

    if (cast_node->expression)
        zenit_node_free(cast_node->expression);

    if (cast_node->type_decl)
        zenit_node_free((ZenitNode*) cast_node->type_decl);

    fl_free(cast_node);
}
