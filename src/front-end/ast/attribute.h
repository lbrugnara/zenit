#ifndef ZENIT_AST_ATTRIBUTE_H
#define ZENIT_AST_ATTRIBUTE_H

#include "node.h"
#include "property-map.h"

/*
 * Struct: ZenitAttributeNode
 *  Represents an attribute in the source code
 * 
 * Members:
 *  <ZenitNode> base: Basic information of the node object
 *  <char> *name: The attribute name
 *  <ZenitPropertyNodeMap> *properties: Map of properties of the attribute
 */
typedef struct ZenitAttributeNode {
    ZenitNode base;
    char *name;
    ZenitPropertyNodeMap *properties;
} ZenitAttributeNode;

/*
 * Function: zenit_attribute_node_new
 *  Creates a new AST node that represents an attribute
 *
 * Parameters:
 *  <ZenitSourceLocation> location: Location information about the attribute
 *  <char> *name: The attribute name
 *
 * Returns:
 *  ZenitAttributeNode*: Attribute node
 *
 * Notes:
 *  The object returned by this function must be freed using the
 *  <zenit_attribute_node_free> function
 */
ZenitAttributeNode* zenit_attribute_node_new(ZenitSourceLocation location, char *name);

/*
 * Function: zenit_attribute_node_uid
 *  Returns a UID for the attribute node
 *
 * Parameters:
 *  <ZenitAttributeNode> *attribute: Attribute node
 *
 * Returns:
 *  char*: UID of the attribute node
 *
 * Notes:
 *  The object returned by this function must be freed using the
 *  <fl_cstring_free> function
 */
char* zenit_attribute_node_uid(ZenitAttributeNode *attribute);

/*
 * Function: zenit_attribute_node_dump
 *  Appends a dump of the attribute node to the output pointer, and
 *  returns a pointer to the -possibly reallocated- output
 *
 * Parameters:
 *  <ZenitAttributeNode> *attribute: Attribute node to dump to the output
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
 *      output = zenit_attribute_node_dump(attribute, output);
 *  If the memory of *output* cannot be reallocated this function frees the memory.
 */
char* zenit_attribute_node_dump(ZenitAttributeNode *attribute, char *output);

/*
 * Function: zenit_attribute_node_free
 *  Frees the memory used by the attribute node
 *
 * Parameters:
 *  <ZenitAttributeNode> *attribute: Attribute node
 *
 * Returns:
 *  void: This function does not return a value
 */
void zenit_attribute_node_free(ZenitAttributeNode *attribute_node);

#endif /* ZENIT_AST_ATTRIBUTE_H */
