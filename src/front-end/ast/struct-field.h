#ifndef ZENIT_AST_FIELD_H
#define ZENIT_AST_FIELD_H

#include "node.h"

/*
 * Struct: ZenitStructFieldNode
 *  An AST node that represents a field (of a struct) initialization
 * 
 * Members:
 *  <ZenitNode> base: Basic information of the node object
 *  <char> *name: The field name
 *  <ZenitNode> *value: The field's value
 *  <ZenitNode> *owner: The field's parent node
 * 
 */
typedef struct ZenitStructFieldNode {
    ZenitNode base;
    char *name;
    ZenitNode *value;
    ZenitNode *owner;
} ZenitStructFieldNode;

/*
 * Function: zenit_struct_field_node_new
 *  Creates a new AST node that represents a field initialization
 *
 * Parameters:
 *  <ZenitSourceLocation> location: Location information about the field initialization
 *  <char> *name: Field name
 *
 * Returns:
 *  ZenitStructFieldNode*: Field initialization node
 *
 * Notes:
 *  The object returned by this function must be freed using the
 *  <zenit_struct_field_node_free> function
 */
ZenitStructFieldNode* zenit_struct_field_node_new(ZenitSourceLocation location, char *name);

/*
 * Function: zenit_struct_field_node_uid
 *  Returns a UID for the field initialization node
 *
 * Parameters:
 *  <ZenitStructFieldNode> *field: Field initialization node
 *
 * Returns:
 *  char*: UID of the field initialization node
 *
 * Notes:
 *  The object returned by this function must be freed using the
 *  <fl_cstring_free> function
 */
char* zenit_struct_field_node_uid(ZenitStructFieldNode *field);

/*
 * Function: zenit_struct_field_node_dump
 *  Appends a dump of the field initialization node to the output pointer, and
 *  returns a pointer to the -possibly reallocated- output
 *
 * Parameters:
 *  <ZenitStructFieldNode> *field: Field initialization node to dump to the output
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
 *      output = zenit_struct_field_node_dump(field, output);
 *  If the memory of *output* cannot be reallocated this function frees the memory.
 */
char* zenit_struct_field_node_dump(ZenitStructFieldNode *field, char *output);

/*
 * Function: zenit_struct_field_node_free
 *  Frees the memory used by the field initialization node
 *
 * Parameters:
 *  <ZenitStructFieldNode> *field: Field initialization node
 *
 * Returns:
 *  void: This function does not return a value
 */
void zenit_struct_field_node_free(ZenitStructFieldNode *field);

#endif /* ZENIT_AST_FIELD_H */
