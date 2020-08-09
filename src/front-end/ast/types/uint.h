#ifndef ZENIT_AST_TYPE_UINT_H
#define ZENIT_AST_TYPE_UINT_H

#include "type.h"
#include "../../types/uint.h"

/*
 * Struct: ZenitUintTypeNode
 *  Represents a uint type declaration in the source code
 * 
 * Members:
 *  <ZenitTypeNode> base: Basic information of the type node object
 *  <ZenitUintTypeSize> size: The size of the uint type
 */
typedef struct ZenitUintTypeNode {
    ZenitTypeNode base;
    ZenitUintTypeSize size;
} ZenitUintTypeNode;

/*
 * Function: zenit_node_type_uint_new
 *  Creates a new AST node that represents a uint type declaration
 *
 * Parameters:
 *  <ZenitSourceLocation> location: Location information about the uint type declaration
 *  <ZenitUintTypeSize> size: The size of the uint type
 * 
 * Returns:
 *  ZenitUintTypeNode*: Uint type declaration node
 *
 * Notes:
 *  The object returned by this function must be freed using the
 *  <zenit_node_type_uint_free> function
 */
ZenitUintTypeNode* zenit_node_type_uint_new(ZenitSourceLocation location, ZenitUintTypeSize size);

/*
 * Function: zenit_node_type_uint_uid
 *  Returns a UID for the uint type declaration node
 *
 * Parameters:
 *  <ZenitUintTypeNode> *uint_type_node: Uint type declaration node
 *
 * Returns:
 *  char*: UID of the uint type declaration node
 *
 * Notes:
 *  The object returned by this function must be freed using the
 *  <fl_cstring_free> function
 */
char* zenit_node_type_uint_uid(ZenitUintTypeNode *uint_type_node);

/*
 * Function: zenit_node_type_uint_dump
 *  Appends a dump of the uint type declaration node to the output pointer, and
 *  returns a pointer to the -possibly reallocated- output
 *
 * Parameters:
 *  <ZenitUintTypeNode> *type_node: Uint type declaration node to dump to the output
 *  <char> *output: Pointer to a heap allocated string
 *
 * Returns:
 *  char*: Pointer to the output string
 *
 * Notes:
 *  Because the *output* pointer can be modified, this function returns
 *  a pointer to the new location in case the memory is reallocated or
 *  to the old location in case the pointer does not need to be modified. Either
 *  way, it is safe to use the function as:
 *      output = zenit_node_type_uint_dump(uint_type, output);
 *  If the memory of *output* cannot be reallocated this function frees the memory.
 */
char* zenit_node_type_uint_dump(ZenitUintTypeNode *type_node, char *output);

/*
 * Function: zenit_node_type_uint_to_string
 *  Returns the string representation of the type declaration
 *
 * Parameters:
 *  <ZenitUintTypeNode> *uint_type_node: The type node object
 *
 * Returns:
 *  char*: The string representation of the type declaration
 *
 * Notes:
 *  The object returned by this function must be freed using the
 *  <fl_cstring_free> function
 */
char* zenit_node_type_uint_to_string(ZenitUintTypeNode *uint_type_node);

/*
 * Function: zenit_node_type_uint_free
 *  Frees the memory used by the uint type declaration node
 *
 * Parameters:
 *  <ZenitUintTypeNode> *uint_type_node: Uint type declaration node
 *
 * Returns:
 *  void: This function does not return a value
 */
void zenit_node_type_uint_free(ZenitUintTypeNode *uint_type_node);

#endif /* ZENIT_AST_TYPE_UINT_H */
