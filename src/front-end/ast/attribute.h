#ifndef ZENIT_AST_ATTRIBUTE_H
#define ZENIT_AST_ATTRIBUTE_H

#include <fllib.h>

#include "node.h"
#include "property_map.h"

/*
 * Struct: struct ZenitAttributeNode
 *  Represents an attribute in the source code
 * 
 * Members:
 *  <struct ZenitNode> base: Basic information of the node object
 *  <char> *name: The attribute name
 *  <struct ZenitPropertyNodeMap> properties: Map of properties of the attribute
 */
struct ZenitAttributeNode {
    struct ZenitNode base;
    char *name;
    struct ZenitPropertyNodeMap properties;
};

/*
 * Function: zenit_node_attribute_new
 *  Creates a new AST node that represents an attribute
 *
 * Parameters:
 *  <struct ZenitSourceLocation> location: Location information about the attribute
 *  <char> *name: The attribute name
 *
 * Returns:
 *  struct ZenitAttributeNode*: Attribute node
 *
 * Notes:
 *  The object returned by this function must be freed using the
 *  <zenit_node_attribute_free> function
 */
struct ZenitAttributeNode* zenit_node_attribute_new(struct ZenitSourceLocation location, char *name);

/*
 * Function: zenit_node_attribute_uid
 *  Returns a UID for the attribute node
 *
 * Parameters:
 *  <struct ZenitAttributeNode> *attribute: Attribute node
 *
 * Returns:
 *  char*: UID of the attribute node
 *
 * Notes:
 *  The object returned by this function must be freed using the
 *  <fl_cstring_free> function
 */
char* zenit_node_attribute_uid(struct ZenitAttributeNode *attribute);

/*
 * Function: zenit_node_attribute_dump
 *  Appends a dump of the attribute node to the output pointer, and
 *  returns a pointer to the -possibly reallocated- output
 *
 * Parameters:
 *  <struct ZenitAttributeNode> *attribute: Attribute node to dump to the output
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
 *      output = zenit_node_attribute_dump(attribute, output);
 *  If the memory of *output* cannot be reallocated this function frees the memory.
 */
char* zenit_node_attribute_dump(struct ZenitAttributeNode *attribute, char *output);

/*
 * Function: zenit_node_attribute_free
 *  Frees the memory used by the attribute node
 *
 * Parameters:
 *  <struct ZenitAttributeNode> *attribute: Attribute node
 *
 * Returns:
 *  void: This function does not return a value
 */
void zenit_node_attribute_free(struct ZenitAttributeNode *attribute_node);

#endif /* ZENIT_AST_ATTRIBUTE_H */
