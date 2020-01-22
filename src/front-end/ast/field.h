#ifndef ZENIT_AST_FIELD_H
#define ZENIT_AST_FIELD_H

#include "node.h"

/*
 * Struct: struct ZenitFieldNode
 *  An AST node that represents a field (of a struct) initialization
 * 
 * Members:
 *  <struct ZenitNode> base: Basic information of the node object
 *  <char> *name: The field name
 *  <struct ZenitNode> *value: The field's value
 *  <struct ZenitNode> *owner: The field's parent node
 * 
 */
struct ZenitFieldNode {
    struct ZenitNode base;
    char *name;
    struct ZenitNode *value;
    struct ZenitNode *owner;
};

/*
 * Function: zenit_node_field_new
 *  Creates a new AST node that represents a field initialization
 *
 * Parameters:
 *  <struct ZenitSourceLocation> location: Location information about the field initialization
 *  <char> *name: Field name
 *
 * Returns:
 *  struct ZenitFieldNode*: Field initialization node
 *
 * Notes:
 *  The object returned by this function must be freed using the
 *  <zenit_node_field_free> function
 */
struct ZenitFieldNode* zenit_node_field_new(struct ZenitSourceLocation location, char *name);

/*
 * Function: zenit_node_field_uid
 *  Returns a UID for the field initialization node
 *
 * Parameters:
 *  <struct ZenitFieldNode> *field: Field initialization node
 *
 * Returns:
 *  char*: UID of the field initialization node
 *
 * Notes:
 *  The object returned by this function must be freed using the
 *  <fl_cstring_free> function
 */
char* zenit_node_field_uid(struct ZenitFieldNode *field);

/*
 * Function: zenit_node_field_dump
 *  Appends a dump of the field initialization node to the output pointer, and
 *  returns a pointer to the -possibly reallocated- output
 *
 * Parameters:
 *  <struct ZenitFieldNode> *field: Field initialization node to dump to the output
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
 *      output = zenit_node_field_dump(field, output);
 *  If the memory of *output* cannot be reallocated this function frees the memory.
 */
char* zenit_node_field_dump(struct ZenitFieldNode *field, char *output);

/*
 * Function: zenit_node_field_free
 *  Frees the memory used by the field initialization node
 *
 * Parameters:
 *  <struct ZenitFieldNode> *field: Field initialization node
 *
 * Returns:
 *  void: This function does not return a value
 */
void zenit_node_field_free(struct ZenitFieldNode *field);

#endif /* ZENIT_AST_FIELD_H */
