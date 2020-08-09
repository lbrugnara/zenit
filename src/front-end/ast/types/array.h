#ifndef ZENIT_AST_TYPE_ARRAY_H
#define ZENIT_AST_TYPE_ARRAY_H

#include "type.h"

/*
 * Struct: ZenitArrayTypeNode
 *  Represents an array type declaration in the source code
 * 
 * Members:
 *  <ZenitTypeNode> base: Basic information of the type node object
 *  <ZenitTypeNode> *member_type: The type declaration of the array members' type
 *  <size_t> length: The number of elements this array type holds
 *  <bool> auto_length: If the array declaration does not declare a length, the length is "auto"
 */
typedef struct ZenitArrayTypeNode {
    ZenitTypeNode base;
    ZenitTypeNode *member_type;
    size_t length;
    bool auto_length;
} ZenitArrayTypeNode;

/*
 * Function: zenit_node_type_array_new
 *  Creates a new AST node that represents an array type declaration
 *
 * Parameters:
 *  <ZenitSourceLocation> location: Location information about the array type declaration
 *  <ZenitTypeNode> *member_type: The type declaration of the array members' type
 * 
 * Returns:
 *  ZenitArrayTypeNode*: Array type declaration node
 *
 * Notes:
 *  The object returned by this function must be freed using the
 *  <zenit_node_type_array_free> function
 */
ZenitArrayTypeNode* zenit_node_type_array_new(ZenitSourceLocation location, ZenitTypeNode *member_type);

/*
 * Function: zenit_node_type_array_uid
 *  Returns a UID for the array type declaration node
 *
 * Parameters:
 *  <ZenitArrayTypeNode> *type_node: Array type declaration node
 *
 * Returns:
 *  char*: UID of the array type declaration node
 *
 * Notes:
 *  The object returned by this function must be freed using the
 *  <fl_cstring_free> function
 */
char* zenit_node_type_array_uid(ZenitArrayTypeNode *type_node);

/*
 * Function: zenit_node_type_array_dump
 *  Appends a dump of the array type declaration node to the output pointer, and
 *  returns a pointer to the -possibly reallocated- output
 *
 * Parameters:
 *  <ZenitArrayTypeNode> *type_node: Array type declaration node to dump to the output
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
 *      output = zenit_node_type_array_dump(array_type, output);
 *  If the memory of *output* cannot be reallocated this function frees the memory.
 */
char* zenit_node_type_array_dump(ZenitArrayTypeNode *type_node, char *output);

/*
 * Function: zenit_node_type_array_to_string
 *  Returns the string representation of the type declaration
 *
 * Parameters:
 *  <ZenitArrayTypeNode> *type_node: The type node object
 *
 * Returns:
 *  char*: The string representation of the type declaration
 *
 * Notes:
 *  The object returned by this function must be freed using the
 *  <fl_cstring_free> function
 */
char* zenit_node_type_array_to_string(ZenitArrayTypeNode *type_node);

/*
 * Function: zenit_node_type_array_free
 *  Frees the memory used by the array type declaration node
 *
 * Parameters:
 *  <ZenitArrayTypeNode> *type_node: Array type declaration node
 *
 * Returns:
 *  void: This function does not return a value
 */
void zenit_node_type_array_free(ZenitArrayTypeNode *type_node);

#endif /* ZENIT_AST_TYPE_ARRAY_H */
