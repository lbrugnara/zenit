#ifndef ZENIT_AST_ATTRIBUTE_MAP_H
#define ZENIT_AST_ATTRIBUTE_MAP_H

#include "attribute.h"

typedef FlHashtable ZenitAttributeNodeMap;

#define zenit_attribute_node_map_new()  \
    ((ZenitAttributeNodeMap*) fl_hashtable_new_args((struct FlHashtableArgs) {  \
            .hash_function = fl_hashtable_hash_string,  \
            .key_allocator = fl_container_allocator_string, \
            .key_comparer = fl_container_equals_string, \
            .key_cleaner = fl_container_cleaner_pointer,    \
            .value_cleaner = (FlContainerCleanupFn) zenit_node_free,  \
            .value_allocator = NULL \
        }))

#define zenit_attribute_node_map_free(mapptr) do { if (mapptr) fl_hashtable_free(mapptr); } while (0)

#define zenit_attribute_node_map_add(mapptr, attr) ((struct ZenitAttributeNode*) fl_hashtable_add(mapptr, attr->name, attr))

#define zenit_attribute_node_map_keys(mapptr) fl_hashtable_keys(mapptr)

#define zenit_attribute_node_map_values(mapptr) ((struct ZenitAttributeNode**) fl_hashtable_values(mapptr))

#define zenit_attribute_node_map_get(mapptr, attrname) ((struct ZenitAttributeNode*) fl_hashtable_get(mapptr, attrname))

#define zenit_attribute_node_map_length(mapptr) fl_hashtable_length(mapptr)

#define zenit_attribute_node_map_has_key(mapptr, attrname) fl_hashtable_has_key(mapptr, attrname)

#endif /* ZENIT_AST_ATTRIBUTE_MAP_H */
