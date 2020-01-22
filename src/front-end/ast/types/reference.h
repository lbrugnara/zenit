#ifndef ZENIT_AST_TYPE_REFERENCE_H
#define ZENIT_AST_TYPE_REFERENCE_H

#include "type.h"

/*
 * Struct: struct ZenitReferenceTypeNode
 *  Represents a reference type declaration in the source code
 * 
 * Members:
 *  <struct ZenitTypeNode> base: Basic information of the type node object
 *  <struct ZenitTypeNode> *element: The type declaration of the referenced element
 */
struct ZenitReferenceTypeNode {
    struct ZenitTypeNode base;
    struct ZenitTypeNode *element;
};

/*
 * Function: zenit_node_type_reference_new
 *  Creates a new AST node that represents a reference type declaration
 *
 * Parameters:
 *  <struct ZenitSourceLocation> location: Location information about the reference type declaration
 *  <struct ZenitTypeNode> *type: The type declaration of the referenced element
 * 
 * Returns:
 *  struct ZenitReferenceTypeNode*: Reference type declaration node
 *
 * Notes:
 *  The object returned by this function must be freed using the
 *  <zenit_node_type_reference_free> function
 */
struct ZenitReferenceTypeNode* zenit_node_type_reference_new(struct ZenitSourceLocation location, struct ZenitTypeNode *type);

/*
 * Function: zenit_node_type_reference_uid
 *  Returns a UID for the reference type declaration node
 *
 * Parameters:
 *  <struct ZenitReferenceTypeNode> *type_node: Reference type declaration node
 *
 * Returns:
 *  char*: UID of the reference type declaration node
 *
 * Notes:
 *  The object returned by this function must be freed using the
 *  <fl_cstring_free> function
 */
char* zenit_node_type_reference_uid(struct ZenitReferenceTypeNode *type_node);

/*
 * Function: zenit_node_type_reference_dump
 *  Appends a dump of the reference type declaration node to the output pointer, and
 *  returns a pointer to the -possibly reallocated- output
 *
 * Parameters:
 *  <struct ZenitReferenceTypeNode> *type_node: Reference type declaration node to dump to the output
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
 *      output = zenit_node_type_reference_dump(ref_type, output);
 *  If the memory of *output* cannot be reallocated this function frees the memory.
 */
char* zenit_node_type_reference_dump(struct ZenitReferenceTypeNode *type_node, char *output);

/*
 * Function: zenit_node_type_reference_to_string
 *  Returns the string representation of the type declaration
 *
 * Parameters:
 *  <struct ZenitReferenceTypeNode> *type_node: The type node object
 *
 * Returns:
 *  char*: The string representation of the type declaration
 *
 * Notes:
 *  The object returned by this function must be freed using the
 *  <fl_cstring_free> function
 */
char* zenit_node_type_reference_to_string(struct ZenitReferenceTypeNode *type_node);

/*
 * Function: zenit_node_type_reference_free
 *  Frees the memory used by the reference type declaration node
 *
 * Parameters:
 *  <struct ZenitReferenceTypeNode> *type_node: Reference type declaration node
 *
 * Returns:
 *  void: This function does not return a value
 */
void zenit_node_type_reference_free(struct ZenitReferenceTypeNode *type_node);

#endif /* ZENIT_AST_TYPE_REFERENCE_H */
