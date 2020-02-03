
#include <fllib/Array.h>
#include <fllib/Cstring.h>
#include "array.h"

struct ZenitArrayNode* zenit_node_array_new(struct ZenitSourceLocation location)
{
    struct ZenitArrayNode *node = fl_malloc(sizeof(struct ZenitArrayNode));
    node->base.nodekind = ZENIT_NODE_ARRAY;
    node->base.location = location;
    node->elements = fl_array_new(sizeof(struct ZenitNode*), 0);

    return node;
}

void zenit_node_array_add_child(struct ZenitArrayNode *array, struct ZenitNode *element)
{
    // Add the node to the elements list
    array->elements = fl_array_append(array->elements, &element);
}

char* zenit_node_array_uid(struct ZenitArrayNode *array)
{
    if (!array)
        return NULL;

    return fl_cstring_vdup("%%L%u:C%u_array", array->base.location.line, array->base.location.col);
}

char* zenit_node_array_dump(struct ZenitArrayNode *array, char *output)
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
 f Function: zenit_node_array_free
 *  Releases the memory of an array node object
 *
 * Parameters:
 *  <struct ZenitArrayNode> *array: Array node object
 *
 * Returns:
 *  <void>: This function does not return a value
 */
void zenit_node_array_free(struct ZenitArrayNode *array)
{
    if (!array)
        return;

    fl_array_free_each_pointer(array->elements, (FlArrayFreeElementFunc) zenit_node_free);

    fl_free(array);
}
