#ifndef ZENIT_AST_PROPERTY_H
#define ZENIT_AST_PROPERTY_H

#include "node.h"

/*
 * Struct: ZenitPropertyNode
 *  Represents an attribute's property in the source code
 * 
 * Members:
 *  <ZenitNode> base: Basic information of the node object
 *  <char> *name: The property name
 *  <ZenitNode> *value: A node that represents the expression to initialize the property
 */
typedef struct ZenitPropertyNode {
    ZenitNode base;
    char *name;
    ZenitNode *value;
} ZenitPropertyNode;

/*
 * Function: zenit_node_property_new
 *  Creates a new AST node that represents an attribute's property
 *
 * Parameters:
 *  <ZenitSourceLocation> location: Location information about the property
 *  <char> *name: The property name
 *  <ZenitNode> *value: The node that represents the value of the property
 *
 * Returns:
 *  ZenitPropertyNode*: Property node
 *
 * Notes:
 *  The object returned by this function must be freed using the
 *  <zenit_node_property_free> function
 */
ZenitPropertyNode* zenit_node_property_new(ZenitSourceLocation location, char *name, ZenitNode *value);

/*
 * Function: zenit_node_property_uid
 *  Returns a UID for the property node
 *
 * Parameters:
 *  <ZenitPropertyNode> *property: Property node
 *
 * Returns:
 *  char*: UID of the property node
 *
 * Notes:
 *  The object returned by this function must be freed using the
 *  <fl_cstring_free> function
 */
char* zenit_node_property_uid(ZenitPropertyNode *property);

/*
 * Function: zenit_node_property_dump
 *  Appends a dump of the property node to the output pointer, and
 *  returns a pointer to the -possibly reallocated- output
 *
 * Parameters:
 *  <ZenitPropertyNode> *property: Property node to dump to the output
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
 *      output = zenit_node_property_dump(property, output);
 *  If the memory of *output* cannot be reallocated this function frees the memory.
 */
char* zenit_node_property_dump(ZenitPropertyNode *property, char *output);

/*
 * Function: zenit_node_property_free
 *  Frees the memory used by the property node
 *
 * Parameters:
 *  <ZenitPropertyNode> *property: Property node
 *
 * Returns:
 *  void: This function does not return a value
 */
void zenit_node_property_free(ZenitPropertyNode *node);

#endif /* ZENIT_AST_T_PROPERTY_H */
