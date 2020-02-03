#ifndef ZENIT_AST_TYPE_H
#define ZENIT_AST_TYPE_H

#include <fllib/Cstring.h>
#include "../node.h"

/*
 * Struct: struct ZenitTypeNode
 *  The base type node object. All the specific type node objects are compound with this one
 * 
 * Members:
 *  <struct ZenitNode> base: Basic information of the node object
 *  <enum ZenitNodeKind> typekind: The specific native type kind
 */
struct ZenitTypeNode {
    struct ZenitNode base;
    enum ZenitTypeKind typekind;
};

#include "array.h"
#include "uint.h"
#include "reference.h"
#include "struct.h"

/*
 * Function: zenit_node_type_to_string
 *  Returns a string representation of the type node.
 *
 * Parameters:
 *  <struct ZenitTypeNode> *typenode: Type node object.
 *
 * Returns:
 *  <char>*: String representation of the *typenode* object
 *
 * Notes:
 *  The object returned by this function must be freed using the
 *  <fl_cstring_Free> function
 */
static char* zenit_node_type_to_string(struct ZenitTypeNode *typenode)
{
    if (!typenode)
        return fl_cstring_dup("<unknown>");

    if (typenode->base.nodekind == ZENIT_NODE_TYPE_STRUCT)
        return zenit_node_type_struct_to_string((struct ZenitStructTypeNode*) typenode);
    
    if (typenode->base.nodekind == ZENIT_NODE_TYPE_REFERENCE)
        return zenit_node_type_reference_to_string((struct ZenitReferenceTypeNode*) typenode);
    
    if (typenode->base.nodekind == ZENIT_NODE_TYPE_ARRAY)
        return zenit_node_type_array_to_string((struct ZenitArrayTypeNode*) typenode);
    
    if (typenode->base.nodekind == ZENIT_NODE_TYPE_UINT)
        return zenit_node_type_uint_to_string((struct ZenitUintTypeNode*) typenode);
        
    return fl_cstring_dup("<unknown>");
}

#endif /* ZENIT_AST_TYPE_H */
