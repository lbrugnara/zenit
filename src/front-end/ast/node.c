#include "array.h"
#include "attribute.h"
#include "cast.h"
#include "identifier.h"
#include "primitive.h"
#include "property.h"
#include "reference.h"
#include "type.h"
#include "variable.h"

/*
 * Function: free_node_array_entry
 *  This function takes a void pointer that is expected to be
 *  a pointer to a <struct ZenitNode> pointer
 */
static inline void free_node_array_entry(void *nodeptr)
{
    if (!nodeptr)
        return;

    struct ZenitNode *node = *(struct ZenitNode**)nodeptr;

    if (!node)
        return;

    zenit_node_free(node);
}

char* zenit_node_print_type(struct ZenitNode *node)
{
    if (!node)
        return "null";

    switch (node->type)
    {
        case ZENIT_NODE_PRIMITIVE:
            return "literal";
        case ZENIT_NODE_VARIABLE:
            return "var";
        case ZENIT_NODE_ARRAY:
            return "array";
        case ZENIT_NODE_REFERENCE:
            return "ref";
        case ZENIT_NODE_IDENTIFIER:
            return "id";
        case ZENIT_NODE_ATTRIBUTE:
            return "attr";
        case ZENIT_NODE_PROPERTY:
            return "prop";
        case ZENIT_NODE_CAST:
            return "cast";
        
        default:
            break;
    }

    return "unknown";
}

/*
 * Function: zenit_node_free
 *  Checks the node's *type* property to call the specific
 *  function that releases its memory.
 */
void zenit_node_free(struct ZenitNode *node)
{
    if (!node)
        return;

    switch (node->type)
    {
        case ZENIT_NODE_PRIMITIVE:
            zenit_node_primitive_free((struct ZenitPrimitiveNode*)node);
            break;

        case ZENIT_NODE_VARIABLE:
            zenit_node_variable_free((struct ZenitVariableNode*)node);
            break;

        case ZENIT_NODE_ARRAY:
            zenit_node_array_free((struct ZenitArrayNode*)node);
            break;

        case ZENIT_NODE_REFERENCE:
            zenit_node_reference_free((struct ZenitReferenceNode*)node);
            break;

        case ZENIT_NODE_IDENTIFIER:
            zenit_node_identifier_free((struct ZenitIdentifierNode*)node);
            break;

        case ZENIT_NODE_ATTRIBUTE:
            zenit_node_attribute_free((struct ZenitAttributeNode*)node);
            break;

        case ZENIT_NODE_PROPERTY:
            zenit_node_property_free((struct ZenitPropertyNode*)node);
            break;

        case ZENIT_NODE_CAST:
            zenit_node_cast_free((struct ZenitCastNode*)node);
            break;

        case ZENIT_NODE_TYPE_ARRAY:
            zenit_node_type_array_free((struct ZenitArrayTypeNode*)node);
            break;

        case ZENIT_NODE_TYPE_PRIMITIVE:
            zenit_node_type_primitive_free((struct ZenitPrimitiveTypeNode*)node);
            break;

        case ZENIT_NODE_TYPE_REFERENCE:
            zenit_node_type_reference_free((struct ZenitReferenceTypeNode*)node);
            break;

        case ZENIT_NODE_TYPE_STRUCT:
            zenit_node_type_struct_free((struct ZenitStructTypeNode*)node);
            break;
    }
}
