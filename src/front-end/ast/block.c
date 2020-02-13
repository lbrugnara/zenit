
#include <fllib/Array.h>
#include <fllib/Cstring.h>
#include "block.h"

struct ZenitBlockNode* zenit_node_block_new(struct ZenitSourceLocation location)
{
    struct ZenitBlockNode *node = fl_malloc(sizeof(struct ZenitBlockNode));
    node->base.nodekind = ZENIT_NODE_BLOCK;
    node->base.location = location;
    node->statements = fl_array_new(sizeof(struct ZenitNode*), 0);

    return node;
}

void zenit_node_block_add_statement(struct ZenitBlockNode *block_node, struct ZenitNode *statement)
{
    // Add the node to the statements list
    block_node->statements = fl_array_append(block_node->statements, &statement);
}

char* zenit_node_block_uid(struct ZenitBlockNode *block_node)
{
    if (!block_node)
        return NULL;

    return fl_cstring_vdup("%%L%u:C%u_block", block_node->base.location.line, block_node->base.location.col);
}

char* zenit_node_block_dump(struct ZenitBlockNode *block_node, char *output)
{
    fl_cstring_append(&output, "(");

    size_t length = fl_array_length(block_node->statements);

    if (length > 0)
    {
        for (size_t i=0; i < length; i++)
        {
            output = zenit_node_dump(block_node->statements[i], output);
            if (i != length - 1)
                fl_cstring_append(&output, " ");
        }
    }

    fl_cstring_append(&output, ")");

    return output;
}

/*
 f Function: zenit_node_block_free
 *  Releases the memory of an array node object
 *
 * Parameters:
 *  <struct ZenitBlockNode> *array: Array node object
 *
 * Returns:
 *  <void>: This function does not return a value
 */
void zenit_node_block_free(struct ZenitBlockNode *array)
{
    if (!array)
        return;

    fl_array_free_each_pointer(array->statements, (FlArrayFreeElementFunc) zenit_node_free);

    fl_free(array);
}
