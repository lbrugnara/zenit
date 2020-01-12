#ifndef ZENIT_AST_TYPE_H
#define ZENIT_AST_TYPE_H

#include "../node.h"

struct ZenitTypeNode {
    struct ZenitNode base;
    enum ZenitType type;
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
 *  The string returned by this function must be freed with
 *  the fl_cstring_free or the free functions
 */
static char* zenit_node_type_to_string(struct ZenitTypeNode *typenode)
{
    if (!typenode)
        return fl_cstring_dup("<unknown>");

    if (typenode->base.type == ZENIT_NODE_TYPE_STRUCT)
        return zenit_node_type_struct_to_string((struct ZenitStructTypeNode*) typenode);
    
    if (typenode->base.type == ZENIT_NODE_TYPE_REFERENCE)
        return zenit_node_type_reference_to_string((struct ZenitReferenceTypeNode*) typenode);
    
    if (typenode->base.type == ZENIT_NODE_TYPE_ARRAY)
        return zenit_node_type_array_to_string((struct ZenitArrayTypeNode*) typenode);
    
    if (typenode->base.type == ZENIT_NODE_TYPE_UINT)
        return zenit_node_type_uint_to_string((struct ZenitUintTypeNode*) typenode);
        
    return fl_cstring_dup("<unknown>");
}

#endif /* ZENIT_AST_TYPE_H */
