
#include <fllib/Array.h>
#include <fllib/Cstring.h>
#include "array.h"

ZenitArrayNode* zenit_array_node_new(ZenitSourceLocation location)
{
    ZenitArrayNode *node = fl_malloc(sizeof(ZenitArrayNode));
    node->base.nodekind = ZENIT_AST_NODE_ARRAY;
    node->base.location = location;
    node->elements = fl_array_new(sizeof(ZenitNode*), 0);

    return node;
}

void zenit_array_node_add_child(ZenitArrayNode *array, ZenitNode *element)
{
    // Add the node to the elements list
    array->elements = fl_array_append(array->elements, &element);
}

char* zenit_array_node_uid(ZenitArrayNode *array)
{
    if (!array)
        return NULL;

    return fl_cstring_vdup("%%L%u:C%u_array", array->base.location.line, array->base.location.col);
}

char* zenit_array_node_dump(ZenitArrayNode *array, char *output)
{
    fl_cstring_append(&output, "(array");

    size_t length = fl_array_length(array->elements);

    if (length > 0)
    {
        fl_cstring_append(&output, " ");
        for (size_t i=0; i < length; i++)
        {
            output = zenit_node_dump(array->elements[i], output);
            if (i != length - 1)
                fl_cstring_append(&output, " ");
        }
    }

    fl_cstring_append(&output, ")");

    return output;
}

/*
 f Function: zenit_array_node_free
 *  Releases the memory of an array node object
 *
 * Parameters:
 *  <ZenitArrayNode> *array: Array node object
 *
 * Returns:
 *  <void>: This function does not return a value
 */
void zenit_array_node_free(ZenitArrayNode *array)
{
    if (!array)
        return;

    fl_array_free_each_pointer(array->elements, (FlArrayFreeElementFunc) zenit_node_free);

    fl_free(array);
}
