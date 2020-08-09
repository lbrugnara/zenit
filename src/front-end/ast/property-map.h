#ifndef ZENIT_AST_PROPERTY_MAP_H
#define ZENIT_AST_PROPERTY_MAP_H

#include <fllib/containers/Hashtable.h>
#include "property.h"

typedef FlHashtable ZenitPropertyNodeMap;

#define zenit_property_node_map_new()   \
    ((ZenitPropertyNodeMap *) fl_hashtable_new_args((struct FlHashtableArgs) {  \
            .hash_function = fl_hashtable_hash_string,  \
            .key_allocator = fl_container_allocator_string, \
            .key_comparer = fl_container_equals_string, \
            .key_cleaner = fl_container_cleaner_pointer,    \
            .value_cleaner = (FlContainerCleanupFn)zenit_node_free,   \
            .value_allocator = NULL \
        }))

#define zenit_property_node_map_free(property_map) do { if (property_map) fl_hashtable_free(property_map); } while(0)

#define zenit_property_node_map_add(property_map, property) ((ZenitPropertyNode*) fl_hashtable_add(property_map, property->name, property))

#define zenit_property_node_map_keys(property_map) fl_hashtable_keys(property_map)

#define zenit_property_node_map_values(property_map) ((ZenitPropertyNode**) fl_hashtable_values(property_map))

#define zenit_property_node_map_get(property_map, property_name) ((ZenitPropertyNode*) fl_hashtable_get(property_map, property_name))

#define zenit_property_node_map_length(property_map) fl_hashtable_length(property_map)

#define zenit_property_node_map_has_key(property_map, property_name) fl_hashtable_has_key(property_map, property_name)

#endif /* ZENIT_AST_PROPERTY_MAP_H */
