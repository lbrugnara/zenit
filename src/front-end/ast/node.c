#include "array.h"
#include "attribute.h"
#include "cast.h"
#include "field.h"
#include "field_decl.h"
#include "identifier.h"
#include "property.h"
#include "reference.h"
#include "struct.h"
#include "struct_decl.h"
#include "types/type.h"
#include "uint.h"
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

char* zenit_node_uid(struct ZenitNode *node)
{
    if (!node)
        return NULL;

    switch (node->nodekind)
    {
        case ZENIT_NODE_UINT:
            return zenit_node_uint_uid((struct ZenitUintNode*)node);

        case ZENIT_NODE_VARIABLE:
            return zenit_node_variable_uid((struct ZenitVariableNode*)node);

        case ZENIT_NODE_STRUCT_DECL:
            return zenit_node_struct_decl_uid((struct ZenitStructDeclNode*)node);

        case ZENIT_NODE_FIELD_DECL:
            return zenit_node_field_decl_uid((struct ZenitFieldDeclNode*)node);

        case ZENIT_NODE_ARRAY:
            return zenit_node_array_uid((struct ZenitArrayNode*)node);

        case ZENIT_NODE_REFERENCE:
            return zenit_node_reference_uid((struct ZenitReferenceNode*)node);

        case ZENIT_NODE_IDENTIFIER:
            return zenit_node_identifier_uid((struct ZenitIdentifierNode*)node);

        case ZENIT_NODE_ATTRIBUTE:
            return zenit_node_attribute_uid((struct ZenitAttributeNode*)node);

        case ZENIT_NODE_PROPERTY:
            return zenit_node_property_uid((struct ZenitPropertyNode*)node);

        case ZENIT_NODE_CAST:
            return zenit_node_cast_uid((struct ZenitCastNode*)node);

        case ZENIT_NODE_STRUCT:
            return zenit_node_struct_uid((struct ZenitStructNode*)node);

        case ZENIT_NODE_FIELD:
            return zenit_node_field_uid((struct ZenitFieldNode*)node);

        case ZENIT_NODE_TYPE_ARRAY:
            return zenit_node_type_array_uid((struct ZenitArrayTypeNode*)node);

        case ZENIT_NODE_TYPE_UINT:
            return zenit_node_type_uint_uid((struct ZenitUintTypeNode*)node);

        case ZENIT_NODE_TYPE_REFERENCE:
            return zenit_node_type_reference_uid((struct ZenitReferenceTypeNode*)node);

        case ZENIT_NODE_TYPE_STRUCT:
            return zenit_node_type_struct_uid((struct ZenitStructTypeNode*)node);
    }

    return NULL;
}

char* zenit_node_dump(struct ZenitNode *node, char *output)
{
    if (!node)
        return NULL;

    switch (node->nodekind)
    {
        case ZENIT_NODE_UINT:
            return zenit_node_uint_dump((struct ZenitUintNode*)node, output);

        case ZENIT_NODE_VARIABLE:
            return zenit_node_variable_dump((struct ZenitVariableNode*)node, output);

        case ZENIT_NODE_STRUCT_DECL:
            return zenit_node_struct_decl_dump((struct ZenitStructDeclNode*)node, output);

        case ZENIT_NODE_FIELD_DECL:
            return zenit_node_field_decl_dump((struct ZenitFieldDeclNode*)node, output);

        case ZENIT_NODE_ARRAY:
            return zenit_node_array_dump((struct ZenitArrayNode*)node, output);

        case ZENIT_NODE_REFERENCE:
            return zenit_node_reference_dump((struct ZenitReferenceNode*)node, output);

        case ZENIT_NODE_IDENTIFIER:
            return zenit_node_identifier_dump((struct ZenitIdentifierNode*)node, output);

        case ZENIT_NODE_ATTRIBUTE:
            return zenit_node_attribute_dump((struct ZenitAttributeNode*)node, output);

        case ZENIT_NODE_PROPERTY:
            return zenit_node_property_dump((struct ZenitPropertyNode*)node, output);

        case ZENIT_NODE_CAST:
            return zenit_node_cast_dump((struct ZenitCastNode*)node, output);

        case ZENIT_NODE_STRUCT:
            return zenit_node_struct_dump((struct ZenitStructNode*)node, output);

        case ZENIT_NODE_FIELD:
            return zenit_node_field_dump((struct ZenitFieldNode*)node, output);

        case ZENIT_NODE_TYPE_ARRAY:
            return zenit_node_type_array_dump((struct ZenitArrayTypeNode*)node, output);

        case ZENIT_NODE_TYPE_UINT:
            return zenit_node_type_uint_dump((struct ZenitUintTypeNode*)node, output);

        case ZENIT_NODE_TYPE_REFERENCE:
            return zenit_node_type_reference_dump((struct ZenitReferenceTypeNode*)node, output);

        case ZENIT_NODE_TYPE_STRUCT:
            return zenit_node_type_struct_dump((struct ZenitStructTypeNode*)node, output);
    }

    return output;
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

    switch (node->nodekind)
    {
        case ZENIT_NODE_UINT:
            zenit_node_uint_free((struct ZenitUintNode*)node);
            break;

        case ZENIT_NODE_VARIABLE:
            zenit_node_variable_free((struct ZenitVariableNode*)node);
            break;

        case ZENIT_NODE_STRUCT_DECL:
            zenit_node_struct_decl_free((struct ZenitStructDeclNode*)node);
            break;

        case ZENIT_NODE_FIELD_DECL:
            zenit_node_field_decl_free((struct ZenitFieldDeclNode*)node);
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

        case ZENIT_NODE_STRUCT:
            zenit_node_struct_free((struct ZenitStructNode*)node);
            break;

        case ZENIT_NODE_FIELD:
            zenit_node_field_free((struct ZenitFieldNode*)node);
            break;

        case ZENIT_NODE_TYPE_ARRAY:
            zenit_node_type_array_free((struct ZenitArrayTypeNode*)node);
            break;

        case ZENIT_NODE_TYPE_UINT:
            zenit_node_type_uint_free((struct ZenitUintTypeNode*)node);
            break;

        case ZENIT_NODE_TYPE_REFERENCE:
            zenit_node_type_reference_free((struct ZenitReferenceTypeNode*)node);
            break;

        case ZENIT_NODE_TYPE_STRUCT:
            zenit_node_type_struct_free((struct ZenitStructTypeNode*)node);
            break;
    }
}
