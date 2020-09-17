#include "array.h"
#include "block.h"
#include "attribute.h"
#include "bool.h"
#include "cast.h"
#include "identifier.h"
#include "if.h"
#include "property.h"
#include "reference.h"
#include "struct-field.h"
#include "struct.h"
#include "struct-field-decl.h"
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
        case ZENIT_AST_NODE_UINT:
            return zenit_uint_node_uid((ZenitUintNode*) node);

        case ZENIT_AST_NODE_BOOL:
            return zenit_bool_node_uid((ZenitBoolNode*) node);

        case ZENIT_AST_NODE_IF:
            return zenit_if_node_uid((ZenitIfNode*) node);

        case ZENIT_AST_NODE_BLOCK:
            return zenit_block_node_uid((ZenitBlockNode*) node);

        case ZENIT_AST_NODE_VARIABLE:
            return zenit_variable_node_uid((ZenitVariableNode*) node);

        case ZENIT_AST_NODE_STRUCT_DECL:
            return zenit_struct_decl_node_uid((ZenitStructDeclNode*) node);

        case ZENIT_AST_NODE_FIELD_DECL:
            return zenit_struct_field_decl_node_uid((ZenitStructFieldDeclNode*) node);

        case ZENIT_AST_NODE_ARRAY:
            return zenit_array_node_uid((ZenitArrayNode*) node);

        case ZENIT_AST_NODE_REFERENCE:
            return zenit_reference_node_uid((ZenitReferenceNode*) node);

        case ZENIT_AST_NODE_IDENTIFIER:
            return zenit_identifier_node_uid((ZenitIdentifierNode*) node);

        case ZENIT_AST_NODE_ATTRIBUTE:
            return zenit_attribute_node_uid((ZenitAttributeNode*) node);

        case ZENIT_AST_NODE_PROPERTY:
            return zenit_property_node_uid((ZenitPropertyNode*) node);

        case ZENIT_AST_NODE_CAST:
            return zenit_cast_node_uid((ZenitCastNode*) node);

        case ZENIT_AST_NODE_STRUCT:
            return zenit_struct_node_uid((ZenitStructNode*) node);

        case ZENIT_AST_NODE_FIELD:
            return zenit_struct_field_node_uid((ZenitStructFieldNode*) node);

        case ZENIT_AST_NODE_TYPE_UINT:
            return zenit_uint_type_node_uid((ZenitUintTypeNode*) node);

        case ZENIT_AST_NODE_TYPE_BOOL:
            return zenit_bool_type_node_uid((ZenitBoolTypeNode*) node);

        case ZENIT_AST_NODE_TYPE_ARRAY:
            return zenit_array_type_node_uid((ZenitArrayTypeNode*) node);

        case ZENIT_AST_NODE_TYPE_REFERENCE:
            return zenit_reference_type_node_uid((ZenitReferenceTypeNode*) node);

        case ZENIT_AST_NODE_TYPE_STRUCT:
            return zenit_struct_type_node_uid((ZenitStructTypeNode*) node);
    }

    return NULL;
}

char* zenit_node_dump(ZenitNode *node, char *output)
{
    if (!node)
        return NULL;

    switch (node->nodekind)
    {
        case ZENIT_AST_NODE_UINT:
            return zenit_uint_node_dump((ZenitUintNode*) node, output);

        case ZENIT_AST_NODE_BOOL:
            return zenit_bool_node_dump((ZenitBoolNode*) node, output);

        case ZENIT_AST_NODE_IF:
            return zenit_if_node_dump((ZenitIfNode*) node, output);

        case ZENIT_AST_NODE_BLOCK:
            return zenit_block_node_dump((ZenitBlockNode*) node, output);

        case ZENIT_AST_NODE_VARIABLE:
            return zenit_variable_node_dump((ZenitVariableNode*) node, output);

        case ZENIT_AST_NODE_STRUCT_DECL:
            return zenit_struct_decl_node_dump((ZenitStructDeclNode*) node, output);

        case ZENIT_AST_NODE_FIELD_DECL:
            return zenit_struct_field_decl_node_dump((ZenitStructFieldDeclNode*) node, output);

        case ZENIT_AST_NODE_ARRAY:
            return zenit_array_node_dump((ZenitArrayNode*) node, output);

        case ZENIT_AST_NODE_REFERENCE:
            return zenit_reference_node_dump((ZenitReferenceNode*) node, output);

        case ZENIT_AST_NODE_IDENTIFIER:
            return zenit_identifier_node_dump((ZenitIdentifierNode*) node, output);

        case ZENIT_AST_NODE_ATTRIBUTE:
            return zenit_attribute_node_dump((ZenitAttributeNode*) node, output);

        case ZENIT_AST_NODE_PROPERTY:
            return zenit_property_node_dump((ZenitPropertyNode*) node, output);

        case ZENIT_AST_NODE_CAST:
            return zenit_cast_node_dump((ZenitCastNode*) node, output);

        case ZENIT_AST_NODE_STRUCT:
            return zenit_struct_node_dump((ZenitStructNode*) node, output);

        case ZENIT_AST_NODE_FIELD:
            return zenit_struct_field_node_dump((ZenitStructFieldNode*) node, output);        

        case ZENIT_AST_NODE_TYPE_UINT:
            return zenit_uint_type_node_dump((ZenitUintTypeNode*) node, output);

        case ZENIT_AST_NODE_TYPE_BOOL:
            return zenit_bool_type_node_dump((ZenitBoolTypeNode*) node, output);

        case ZENIT_AST_NODE_TYPE_ARRAY:
            return zenit_array_type_node_dump((ZenitArrayTypeNode*) node, output);

        case ZENIT_AST_NODE_TYPE_REFERENCE:
            return zenit_reference_type_node_dump((ZenitReferenceTypeNode*) node, output);

        case ZENIT_AST_NODE_TYPE_STRUCT:
            return zenit_struct_type_node_dump((ZenitStructTypeNode*) node, output);
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
        case ZENIT_AST_NODE_UINT:
            zenit_uint_node_free((ZenitUintNode*) node);
            break;

        case ZENIT_AST_NODE_BOOL:
            zenit_bool_node_free((ZenitBoolNode*) node);
            break;

        case ZENIT_AST_NODE_IF:
            zenit_if_node_free((ZenitIfNode*) node);
            break;

        case ZENIT_AST_NODE_BLOCK:
            zenit_block_node_free((ZenitBlockNode*) node);
            break;

        case ZENIT_AST_NODE_VARIABLE:
            zenit_variable_node_free((ZenitVariableNode*) node);
            break;

        case ZENIT_AST_NODE_STRUCT_DECL:
            zenit_struct_decl_node_free((ZenitStructDeclNode*) node);
            break;

        case ZENIT_AST_NODE_FIELD_DECL:
            zenit_struct_field_decl_node_free((ZenitStructFieldDeclNode*) node);
            break;

        case ZENIT_AST_NODE_ARRAY:
            zenit_array_node_free((ZenitArrayNode*) node);
            break;

        case ZENIT_AST_NODE_REFERENCE:
            zenit_reference_node_free((ZenitReferenceNode*) node);
            break;

        case ZENIT_AST_NODE_IDENTIFIER:
            zenit_identifier_node_free((ZenitIdentifierNode*) node);
            break;

        case ZENIT_AST_NODE_ATTRIBUTE:
            zenit_attribute_node_free((ZenitAttributeNode*) node);
            break;

        case ZENIT_AST_NODE_PROPERTY:
            zenit_property_node_free((ZenitPropertyNode*) node);
            break;

        case ZENIT_AST_NODE_CAST:
            zenit_cast_node_free((ZenitCastNode*) node);
            break;

        case ZENIT_AST_NODE_STRUCT:
            zenit_struct_node_free((ZenitStructNode*) node);
            break;

        case ZENIT_AST_NODE_FIELD:
            zenit_struct_field_node_free((ZenitStructFieldNode*) node);
            break;

        case ZENIT_AST_NODE_TYPE_UINT:
            zenit_uint_type_node_free((ZenitUintTypeNode*) node);
            break;

        case ZENIT_AST_NODE_TYPE_BOOL:
            zenit_bool_type_node_free((ZenitBoolTypeNode*) node);
            break;

        case ZENIT_AST_NODE_TYPE_ARRAY:
            zenit_array_type_node_free((ZenitArrayTypeNode*) node);
            break;

        case ZENIT_AST_NODE_TYPE_REFERENCE:
            zenit_reference_type_node_free((ZenitReferenceTypeNode*) node);
            break;

        case ZENIT_AST_NODE_TYPE_STRUCT:
            zenit_struct_type_node_free((ZenitStructTypeNode*) node);
            break;
    }
}
