#ifndef ZENIT_AST_PROPERTY_MAP_H
#define ZENIT_AST_PROPERTY_MAP_H

#include <fllib/containers/Hashtable.h>
#include "property.h"

typedef FlHashtable ZenitPropertyNodeMap;

static inline ZenitPropertyNodeMap* zenit_property_node_map_new(void)
{
    return (ZenitPropertyNodeMap*) fl_hashtable_new_args((struct FlHashtableArgs) {
        .hash_function = fl_hashtable_hash_string,
        .key_allocator = fl_container_allocator_string,
        .key_comparer = fl_container_equals_string,
        .key_cleaner = fl_container_cleaner_pointer,
        .value_cleaner = (FlContainerCleanupFn) zenit_node_free,
        .value_allocator = NULL
    });
}

static inline void zenit_property_node_map_free(ZenitPropertyNodeMap *property_map)
{ 
    if (property_map) 
        fl_hashtable_free(property_map); 
}

static inline ZenitPropertyNode* zenit_property_node_map_add(ZenitPropertyNodeMap *property_map, ZenitPropertyNode *property)
{
    return (ZenitPropertyNode*) fl_hashtable_add(property_map, property->name, property);
}

static inline const char** zenit_property_node_map_keys(ZenitPropertyNodeMap *property_map)
{
    return fl_hashtable_keys(property_map);
}

static inline ZenitPropertyNode** zenit_property_node_map_values(ZenitPropertyNodeMap *property_map)
{
    return (ZenitPropertyNode**) fl_hashtable_values(property_map);
}

static inline ZenitPropertyNode* zenit_property_node_map_get(ZenitPropertyNodeMap *property_map, const char *property_name)
{
    return (ZenitPropertyNode*) fl_hashtable_get(property_map, property_name);
}

static inline size_t zenit_property_node_map_length(ZenitPropertyNodeMap *property_map)
{
    return fl_hashtable_length(property_map);
}

static inline bool zenit_property_node_map_has_key(ZenitPropertyNodeMap *property_map, const char *property_name)
{
    return fl_hashtable_has_key(property_map, property_name);
}

#endif /* ZENIT_AST_PROPERTY_MAP_H */
