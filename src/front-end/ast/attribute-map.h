#ifndef ZENIT_AST_ATTRIBUTE_MAP_H
#define ZENIT_AST_ATTRIBUTE_MAP_H

#include "attribute.h"

typedef FlHashtable ZenitAttributeNodeMap;

static inline ZenitAttributeNodeMap* zenit_attribute_node_map_new(void)
{
    return (ZenitAttributeNodeMap*) fl_hashtable_new_args((struct FlHashtableArgs) {
        .hash_function = fl_hashtable_hash_string,
        .key_allocator = fl_container_allocator_string,
        .key_comparer = fl_container_equals_string,
        .key_cleaner = fl_container_cleaner_pointer,  
        .value_cleaner = (FlContainerCleanupFn) zenit_node_free,
        .value_allocator = NULL
    });
}

static inline void zenit_attribute_node_map_free(ZenitAttributeNodeMap *mapptr)
{
    if (mapptr) 
        fl_hashtable_free(mapptr); 
}

static inline ZenitAttributeNode* zenit_attribute_node_map_add(ZenitAttributeNodeMap *mapptr, ZenitAttributeNode *attr)
{
    return (ZenitAttributeNode*) fl_hashtable_add(mapptr, attr->name, attr);
}

static inline const char** zenit_attribute_node_map_keys(ZenitAttributeNodeMap *mapptr)
{
    return fl_hashtable_keys(mapptr);
}

static inline ZenitAttributeNode** zenit_attribute_node_map_values(ZenitAttributeNodeMap *mapptr)
{
    return (ZenitAttributeNode**) fl_hashtable_values(mapptr);
}

static inline ZenitAttributeNode* zenit_attribute_node_map_get(ZenitAttributeNodeMap *mapptr, const char *attrname)
{
    return (ZenitAttributeNode*) fl_hashtable_get(mapptr, attrname);
}

static inline size_t zenit_attribute_node_map_length(ZenitAttributeNodeMap *mapptr)
{
    return fl_hashtable_length(mapptr);
}

static inline bool zenit_attribute_node_map_has_key(ZenitAttributeNodeMap *mapptr, const char *attrname)
{
    return fl_hashtable_has_key(mapptr, attrname);
}

#endif /* ZENIT_AST_ATTRIBUTE_MAP_H */
