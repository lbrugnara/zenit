#ifndef ZENIT_AST_ATTRIBUTE_H
#define ZENIT_AST_ATTRIBUTE_H

#include <fllib.h>

#include "node.h"
#include "property.h"

struct ZenitAttributeNode {
    struct ZenitNode base;
    char *name;
    struct ZenitPropertyNodeMap properties;
};

struct ZenitAttributeNodeMap {
    FlHashtable map;
};

static inline struct ZenitAttributeNodeMap zenit_attribute_node_map_new()
{
    return (struct ZenitAttributeNodeMap) {
        .map = fl_hashtable_new_args((struct FlHashtableArgs) {
            .hash_function = fl_hashtable_hash_string,
            .key_allocator = fl_container_allocator_string,
            .key_comparer = fl_container_equals_string,
            .key_cleaner = fl_container_cleaner_pointer,
            .value_cleaner = (FlContainerCleanupFunction)zenit_node_free,
            .value_allocator = NULL
        })
    };
}

static inline void zenit_attribute_node_map_free(struct ZenitAttributeNodeMap *mapptr)
{
    fl_hashtable_free(mapptr->map);
}

static inline struct ZenitAttributeNode* zenit_attribute_node_map_add(struct ZenitAttributeNodeMap *mapptr, struct ZenitAttributeNode *attr)
{
    return (struct ZenitAttributeNode*)fl_hashtable_add(mapptr->map, attr->name, attr);
}

static inline const char** zenit_attribute_node_map_keys(struct ZenitAttributeNodeMap *mapptr)
{
    return fl_hashtable_keys(mapptr->map);
}

static inline struct ZenitAttributeNode** zenit_attribute_node_map_values(struct ZenitAttributeNodeMap *mapptr)
{
    return fl_hashtable_values(mapptr->map);
}

static inline struct ZenitAttributeNode* zenit_attribute_node_map_get(struct ZenitAttributeNodeMap *mapptr, const char *attrname)
{
    return (struct ZenitAttributeNode*)fl_hashtable_get(mapptr->map, attrname);
}

static inline size_t zenit_attribute_node_map_length(struct ZenitAttributeNodeMap *mapptr)
{
    return fl_hashtable_length(mapptr->map);
}

static inline bool zenit_attribute_node_map_has_key(struct ZenitAttributeNodeMap *mapptr, const char *attrname)
{
    return fl_hashtable_has_key(mapptr->map, attrname);
}

#endif /* ZENIT_AST_ATTRIBUTE_H */
