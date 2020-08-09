#ifndef ZENIT_AST_TYPE_H
#define ZENIT_AST_TYPE_H

#include <fllib/Cstring.h>
#include "../node.h"

/*
 * Struct: ZenitTypeNode
 *  The base type node object. All the specific type node objects are compound with this one
 * 
 * Members:
 *  <ZenitNode> base: Basic information of the node object
 *  <ZenitNodeKind> typekind: The specific native type kind
 */
typedef struct ZenitTypeNode {
    ZenitNode base;
    ZenitTypeKind typekind;
} ZenitTypeNode;

#include "array.h"
#include "bool.h"
#include "uint.h"
#include "reference.h"
#include "struct.h"

/*
 * Function: zenit_node_type_to_string
 *  Returns a string representation of the type node.
 *
 * Parameters:
 *  <ZenitTypeNode> *typenode: Type node object.
 *
 * Returns:
 *  <char>*: String representation of the *typenode* object
 *
 * Notes:
 *  The object returned by this function must be freed using the
 *  <fl_cstring_Free> function
 */
static char* zenit_node_type_to_string(ZenitTypeNode *typenode)
{
    if (!typenode)
        return fl_cstring_dup("<unknown>");

    if (typenode->base.nodekind == ZENIT_NODE_TYPE_STRUCT)
        return zenit_node_type_struct_to_string((ZenitStructTypeNode*) typenode);
    
    if (typenode->base.nodekind == ZENIT_NODE_TYPE_REFERENCE)
        return zenit_node_type_reference_to_string((ZenitReferenceTypeNode*) typenode);
    
    if (typenode->base.nodekind == ZENIT_NODE_TYPE_ARRAY)
        return zenit_node_type_array_to_string((ZenitArrayTypeNode*) typenode);
    
    if (typenode->base.nodekind == ZENIT_NODE_TYPE_UINT)
        return zenit_node_type_uint_to_string((ZenitUintTypeNode*) typenode);

    if (typenode->base.nodekind == ZENIT_NODE_TYPE_BOOL)
        return zenit_node_type_bool_to_string((ZenitBoolTypeNode*) typenode);
        
    return fl_cstring_dup("<unknown>");
}

#endif /* ZENIT_AST_TYPE_H */
