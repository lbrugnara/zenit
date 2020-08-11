#ifndef ZENIT_AST_TYPE_STRUCT_H
#define ZENIT_AST_TYPE_STRUCT_H

#include "type.h"

/*
 * Struct: ZenitStructTypeNode
 *  Represents a struct type declaration in the source code
 * 
 * Members:
 *  <ZenitTypeNode> base: Basic information of the type node object
 *  <char> *name: The struct name
 *  <ZenitTypeNode> **members: The members of the struct type declaration
 */
typedef struct ZenitStructTypeNode {
    ZenitTypeNode base;
    char *name;
    ZenitTypeNode **members;
} ZenitStructTypeNode;

/*
 * Function: zenit_struct_type_node_new
 *  Creates a new AST node that represents a struct type declaration
 *
 * Parameters:
 *  <ZenitSourceLocation> location: Location information about the struct type declaration
 *  <char> *name: The struct name
 * 
 * Returns:
 *  ZenitStructTypeNode*: Struct type declaration node
 *
 * Notes:
 *  The object returned by this function must be freed using the
 *  <zenit_struct_type_node_free> function
 */
ZenitStructTypeNode* zenit_struct_type_node_new(ZenitSourceLocation location, char *name);

/*
 * Function: zenit_struct_type_node_uid
 *  Returns a UID for the struct type declaration node
 *
 * Parameters:
 *  <ZenitStructTypeNode> *type_node: Struct type declaration node
 *
 * Returns:
 *  char*: UID of the struct type declaration node
 *
 * Notes:
 *  The object returned by this function must be freed using the
 *  <fl_cstring_free> function
 */
char* zenit_struct_type_node_uid(ZenitStructTypeNode *type_node);

/*
 * Function: zenit_struct_type_node_dump
 *  Appends a dump of the struct type declaration node to the output pointer, and
 *  returns a pointer to the -possibly reallocated- output
 *
 * Parameters:
 *  <ZenitStructTypeNode> *type_node: Struct type declaration node to dump to the output
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
 *      output = zenit_struct_type_node_dump(struct_type, output);
 *  If the memory of *output* cannot be reallocated this function frees the memory.
 */
char* zenit_struct_type_node_dump(ZenitStructTypeNode *type_node, char *output);

/*
 * Function: zenit_struct_type_node_to_string
 *  Returns the string representation of the type declaration
 *
 * Parameters:
 *  <ZenitStructTypeNode> *type_node: The type node object
 *
 * Returns:
 *  char*: The string representation of the type declaration
 *
 * Notes:
 *  The object returned by this function must be freed using the
 *  <fl_cstring_free> function
 */
char* zenit_struct_type_node_to_string(ZenitStructTypeNode *type_node);

/*
 * Function: zenit_struct_type_node_free
 *  Frees the memory used by the struct type declaration node
 *
 * Parameters:
 *  <ZenitStructTypeNode> *type_node: Struct type declaration node
 *
 * Returns:
 *  void: This function does not return a value
 */
void zenit_struct_type_node_free(ZenitStructTypeNode *type_node);

#endif /* ZENIT_AST_TYPE_STRUCT_H */
