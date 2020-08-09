#include "array.h"
#include "block.h"
#include "attribute.h"
#include "bool.h"
#include "cast.h"
#include "field.h"
#include "field-decl.h"
#include "identifier.h"
#include "if.h"
#include "property.h"
#include "reference.h"
#include "struct.h"
#include "struct-decl.h"
#include "types/type.h"
#include "uint.h"
#include "variable.h"

/*
 * Function: free_node_array_entry
 *  This function takes a void pointer that is expected to be
 *  a pointer to a <ZenitNode> pointer
 */
static inline void free_node_array_entry(void *nodeptr)
{
    if (!nodeptr)
        return;

    ZenitNode *node = *(ZenitNode**) nodeptr;

    if (!node)
        return;

    zenit_node_free(node);
}

char* zenit_node_uid(ZenitNode *node)
{
    if (!node)
        return NULL;

    switch (node->nodekind)
    {
        case ZENIT_NODE_UINT:
            return zenit_node_uint_uid((ZenitUintNode*) node);

        case ZENIT_NODE_BOOL:
            return zenit_node_bool_uid((ZenitBoolNode*) node);

        case ZENIT_NODE_IF:
            return zenit_node_if_uid((ZenitIfNode*) node);

        case ZENIT_NODE_BLOCK:
            return zenit_node_block_uid((ZenitBlockNode*) node);

        case ZENIT_NODE_VARIABLE:
            return zenit_node_variable_uid((ZenitVariableNode*) node);

        case ZENIT_NODE_STRUCT_DECL:
            return zenit_node_struct_decl_uid((ZenitStructDeclNode*) node);

        case ZENIT_NODE_FIELD_DECL:
            return zenit_node_field_decl_uid((ZenitFieldDeclNode*) node);

        case ZENIT_NODE_ARRAY:
            return zenit_node_array_uid((ZenitArrayNode*) node);

        case ZENIT_NODE_REFERENCE:
            return zenit_node_reference_uid((ZenitReferenceNode*) node);

        case ZENIT_NODE_IDENTIFIER:
            return zenit_node_identifier_uid((ZenitIdentifierNode*) node);

        case ZENIT_NODE_ATTRIBUTE:
            return zenit_node_attribute_uid((ZenitAttributeNode*) node);

        case ZENIT_NODE_PROPERTY:
            return zenit_node_property_uid((ZenitPropertyNode*) node);

        case ZENIT_NODE_CAST:
            return zenit_node_cast_uid((ZenitCastNode*) node);

        case ZENIT_NODE_STRUCT:
            return zenit_node_struct_uid((ZenitStructNode*) node);

        case ZENIT_NODE_FIELD:
            return zenit_node_field_uid((ZenitFieldNode*) node);

        case ZENIT_NODE_TYPE_UINT:
            return zenit_node_type_uint_uid((ZenitUintTypeNode*) node);

        case ZENIT_NODE_TYPE_BOOL:
            return zenit_node_type_bool_uid((ZenitBoolTypeNode*) node);

        case ZENIT_NODE_TYPE_ARRAY:
            return zenit_node_type_array_uid((ZenitArrayTypeNode*) node);

        case ZENIT_NODE_TYPE_REFERENCE:
            return zenit_node_type_reference_uid((ZenitReferenceTypeNode*) node);

        case ZENIT_NODE_TYPE_STRUCT:
            return zenit_node_type_struct_uid((ZenitStructTypeNode*) node);
    }

    return NULL;
}

char* zenit_node_dump(ZenitNode *node, char *output)
{
    if (!node)
        return NULL;

    switch (node->nodekind)
    {
        case ZENIT_NODE_UINT:
            return zenit_node_uint_dump((ZenitUintNode*) node, output);

        case ZENIT_NODE_BOOL:
            return zenit_node_bool_dump((ZenitBoolNode*) node, output);

        case ZENIT_NODE_IF:
            return zenit_node_if_dump((ZenitIfNode*) node, output);

        case ZENIT_NODE_BLOCK:
            return zenit_node_block_dump((ZenitBlockNode*) node, output);

        case ZENIT_NODE_VARIABLE:
            return zenit_node_variable_dump((ZenitVariableNode*) node, output);

        case ZENIT_NODE_STRUCT_DECL:
            return zenit_node_struct_decl_dump((ZenitStructDeclNode*) node, output);

        case ZENIT_NODE_FIELD_DECL:
            return zenit_node_field_decl_dump((ZenitFieldDeclNode*) node, output);

        case ZENIT_NODE_ARRAY:
            return zenit_node_array_dump((ZenitArrayNode*) node, output);

        case ZENIT_NODE_REFERENCE:
            return zenit_node_reference_dump((ZenitReferenceNode*) node, output);

        case ZENIT_NODE_IDENTIFIER:
            return zenit_node_identifier_dump((ZenitIdentifierNode*) node, output);

        case ZENIT_NODE_ATTRIBUTE:
            return zenit_node_attribute_dump((ZenitAttributeNode*) node, output);

        case ZENIT_NODE_PROPERTY:
            return zenit_node_property_dump((ZenitPropertyNode*) node, output);

        case ZENIT_NODE_CAST:
            return zenit_node_cast_dump((ZenitCastNode*) node, output);

        case ZENIT_NODE_STRUCT:
            return zenit_node_struct_dump((ZenitStructNode*) node, output);

        case ZENIT_NODE_FIELD:
            return zenit_node_field_dump((ZenitFieldNode*) node, output);        

        case ZENIT_NODE_TYPE_UINT:
            return zenit_node_type_uint_dump((ZenitUintTypeNode*) node, output);

        case ZENIT_NODE_TYPE_BOOL:
            return zenit_node_type_bool_dump((ZenitBoolTypeNode*) node, output);

        case ZENIT_NODE_TYPE_ARRAY:
            return zenit_node_type_array_dump((ZenitArrayTypeNode*) node, output);

        case ZENIT_NODE_TYPE_REFERENCE:
            return zenit_node_type_reference_dump((ZenitReferenceTypeNode*) node, output);

        case ZENIT_NODE_TYPE_STRUCT:
            return zenit_node_type_struct_dump((ZenitStructTypeNode*) node, output);
    }

    return output;
}


/*
 * Function: zenit_node_free
 *  Checks the node's *type* property to call the specific
 *  function that releases its memory.
 */
void zenit_node_free(ZenitNode *node)
{
    if (!node)
        return;

    switch (node->nodekind)
    {
        case ZENIT_NODE_UINT:
            zenit_node_uint_free((ZenitUintNode*) node);
            break;

        case ZENIT_NODE_BOOL:
            zenit_node_bool_free((ZenitBoolNode*) node);
            break;

        case ZENIT_NODE_IF:
            zenit_node_if_free((ZenitIfNode*) node);
            break;

        case ZENIT_NODE_BLOCK:
            zenit_node_block_free((ZenitBlockNode*) node);
            break;

        case ZENIT_NODE_VARIABLE:
            zenit_node_variable_free((ZenitVariableNode*) node);
            break;

        case ZENIT_NODE_STRUCT_DECL:
            zenit_node_struct_decl_free((ZenitStructDeclNode*) node);
            break;

        case ZENIT_NODE_FIELD_DECL:
            zenit_node_field_decl_free((ZenitFieldDeclNode*) node);
            break;

        case ZENIT_NODE_ARRAY:
            zenit_node_array_free((ZenitArrayNode*) node);
            break;

        case ZENIT_NODE_REFERENCE:
            zenit_node_reference_free((ZenitReferenceNode*) node);
            break;

        case ZENIT_NODE_IDENTIFIER:
            zenit_node_identifier_free((ZenitIdentifierNode*) node);
            break;

        case ZENIT_NODE_ATTRIBUTE:
            zenit_node_attribute_free((ZenitAttributeNode*) node);
            break;

        case ZENIT_NODE_PROPERTY:
            zenit_node_property_free((ZenitPropertyNode*) node);
            break;

        case ZENIT_NODE_CAST:
            zenit_node_cast_free((ZenitCastNode*) node);
            break;

        case ZENIT_NODE_STRUCT:
            zenit_node_struct_free((ZenitStructNode*) node);
            break;

        case ZENIT_NODE_FIELD:
            zenit_node_field_free((ZenitFieldNode*) node);
            break;

        case ZENIT_NODE_TYPE_UINT:
            zenit_node_type_uint_free((ZenitUintTypeNode*) node);
            break;

        case ZENIT_NODE_TYPE_BOOL:
            zenit_node_type_bool_free((ZenitBoolTypeNode*) node);
            break;

        case ZENIT_NODE_TYPE_ARRAY:
            zenit_node_type_array_free((ZenitArrayTypeNode*) node);
            break;

        case ZENIT_NODE_TYPE_REFERENCE:
            zenit_node_type_reference_free((ZenitReferenceTypeNode*) node);
            break;

        case ZENIT_NODE_TYPE_STRUCT:
            zenit_node_type_struct_free((ZenitStructTypeNode*) node);
            break;
    }
}
