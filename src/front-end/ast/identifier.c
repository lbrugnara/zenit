#include <fllib.h>
#include "identifier.h"

struct ZenitIdentifierNode* zenit_node_identifier_new(struct ZenitSourceLocation location, char *name)
{
    struct ZenitIdentifierNode *id_node = fl_malloc(sizeof(struct ZenitIdentifierNode));
    id_node->base.type = ZENIT_NODE_IDENTIFIER;
    id_node->base.location = location;
    id_node->name = name;

    return id_node;
}

/*
 * Function: zenit_node_identifier_free
 *  Releases the memory of the <struct ZenitIdentifierNode> object
 *
 * Parameters:
 *  identifier_node - Object to be freed.
 *
 * Returns:
 *  void - This function does not return a value
 */
void zenit_node_identifier_free(struct ZenitIdentifierNode *identifier_node)
{
    if (!identifier_node)
        return;

    fl_cstring_free(identifier_node->name);

    fl_free(identifier_node);
}

