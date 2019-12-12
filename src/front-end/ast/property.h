#ifndef ZENIT_AST_PROPERTY_H
#define ZENIT_AST_PROPERTY_H

#include <fllib.h>

#include "node.h"

struct ZenitPropertyNode {
    struct ZenitNode base;
    char *name;
    struct ZenitNode *value;
};

struct ZenitPropertyNodeMap {
    FlHashtable map;
};

static inline struct ZenitPropertyNode* zenit_node_property_new(struct ZenitSourceLocation location, char *name, struct ZenitNode *value)
{
    struct ZenitPropertyNode *property = fl_malloc(sizeof(struct ZenitPropertyNode));
    property->base.type = ZENIT_NODE_PROPERTY;
    property->base.location = location;
    property->name = name;
    property->value = value;

    return property;
}

static inline char* zenit_node_property_uid(struct ZenitPropertyNode *property)
{
    if (!property)
        return NULL;

    return fl_cstring_vdup("%%L%u_C%u_property[%s]", property->base.location.line, property->base.location.col, property->name);
}

static inline void zenit_node_property_free(struct ZenitPropertyNode *node)
{
    if (!node)
        return;

    if (node->name)
        fl_cstring_free(node->name);

    zenit_node_free(node->value);

    fl_free(node);
}

static inline struct ZenitPropertyNodeMap zenit_property_node_map_new()
{
    return (struct ZenitPropertyNodeMap) {
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

static inline void zenit_property_node_map_free(struct ZenitPropertyNodeMap *property_map)
{
    if (property_map->map)
        fl_hashtable_free(property_map->map);
}

static inline struct ZenitPropertyNode* zenit_property_node_map_add(struct ZenitPropertyNodeMap *property_map, struct ZenitPropertyNode *property)
{
    return (struct ZenitPropertyNode*)fl_hashtable_add(property_map->map, property->name, property);
}

static inline const char** zenit_property_node_map_keys(struct ZenitPropertyNodeMap *property_map)
{
    return fl_hashtable_keys(property_map->map);
}

static inline struct ZenitPropertyNode** zenit_property_node_map_values(struct ZenitPropertyNodeMap *property_map)
{
    return fl_hashtable_values(property_map->map);
}

static inline struct ZenitPropertyNode* zenit_property_node_map_get(struct ZenitPropertyNodeMap *property_map, const char *property_name)
{
    return (struct ZenitPropertyNode*)fl_hashtable_get(property_map->map, property_name);
}

static inline size_t zenit_property_node_map_length(struct ZenitPropertyNodeMap *property_map)
{
    return fl_hashtable_length(property_map->map);
}

static inline bool zenit_property_node_map_has_key(struct ZenitPropertyNodeMap *property_map, const char *property_name)
{
    return fl_hashtable_has_key(property_map->map, property_name);
}

#endif /* ZENIT_AST_T_PROPERTY_H */
