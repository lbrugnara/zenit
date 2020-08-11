#ifndef ZENIT_AST_TYPE_BOOL_H
#define ZENIT_AST_TYPE_BOOL_H

#include "bool.h"
#include "../../types/bool.h"

/*
 * Struct: ZenitBoolTypeNode
 *  Represents a boolean type declaration in the source code
 * 
 * Members:
 *  <ZenitTypeNode> base: Basic information of the type node object
 */
typedef struct ZenitBoolTypeNode {
    ZenitTypeNode base;
} ZenitBoolTypeNode;

/*
 * Function: zenit_bool_type_node_new
 *  Creates a new AST node that represents a boolean type declaration
 *
 * Parameters:
 *  <ZenitSourceLocation> location: Location information about the boolean type declaration
 * 
 * Returns:
 *  ZenitBoolTypeNode*: Boolean type declaration node
 *
 * Notes:
 *  The object returned by this function must be freed using the
 *  <zenit_bool_type_node_free> function
 */
ZenitBoolTypeNode* zenit_bool_type_node_new(ZenitSourceLocation location);

/*
 * Function: zenit_bool_type_node_uid
 *  Returns a UID for the boolean type declaration node
 *
 * Parameters:
 *  <ZenitBoolTypeNode> *bool_type_node: Boolean type declaration node
 *
 * Returns:
 *  char*: UID of the boolean type declaration node
 *
 * Notes:
 *  The object returned by this function must be freed using the
 *  <fl_cstring_free> function
 */
char* zenit_bool_type_node_uid(ZenitBoolTypeNode *bool_type_node);

/*
 * Function: zenit_bool_type_node_dump
 *  Appends a dump of the boolean type declaration node to the output pointer, and
 *  returns a pointer to the -possibly reallocated- output
 *
 * Parameters:
 *  <ZenitBoolTypeNode> *type_node: Boolean type declaration node to dump to the output
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
 *      output = zenit_bool_type_node_dump(bool_type, output);
 *  If the memory of *output* cannot be reallocated this function frees the memory.
 */
char* zenit_bool_type_node_dump(ZenitBoolTypeNode *type_node, char *output);

/*
 * Function: zenit_bool_type_node_to_string
 *  Returns the string representation of the type declaration
 *
 * Parameters:
 *  <ZenitBoolTypeNode> *bool_type_node: The type node object
 *
 * Returns:
 *  char*: The string representation of the type declaration
 *
 * Notes:
 *  The object returned by this function must be freed using the
 *  <fl_cstring_free> function
 */
char* zenit_bool_type_node_to_string(ZenitBoolTypeNode *bool_type_node);

/*
 * Function: zenit_bool_type_node_free
 *  Frees the memory used by the boolean type declaration node
 *
 * Parameters:
 *  <ZenitBoolTypeNode> *bool_type_node: Boolean type declaration node
 *
 * Returns:
 *  void: This function does not return a value
 */
void zenit_bool_type_node_free(ZenitBoolTypeNode *bool_type_node);

#endif /* ZENIT_AST_TYPE_BOOL_H */
