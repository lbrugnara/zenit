#ifndef ZIR_ATTRIBUTE_MAP_H
#define ZIR_ATTRIBUTE_MAP_H

#include <fllib.h>
#include "attribute.h"

struct ZirAttributeMap {
    FlHashtable map;
};

static inline struct ZirAttributeMap zir_attribute_map_new()
{
    return (struct ZirAttributeMap) {
        .map = fl_hashtable_new_args((struct FlHashtableArgs) {
            .hash_function = fl_hashtable_hash_string,
            .key_allocator = fl_container_allocator_string,
            .key_comparer = fl_container_equals_string,
            .key_cleaner = fl_container_cleaner_pointer,
            .value_cleaner = (FlContainerCleanupFunction) zir_attribute_free,
            .value_allocator = NULL
        })
    };
}

static inline void zir_attribute_map_free(struct ZirAttributeMap *attr_map)
{
    if (attr_map->map)
        fl_hashtable_free(attr_map->map);
}

static inline struct ZirAttribute* zir_attribute_map_add(struct ZirAttributeMap *attr_map, struct ZirAttribute *attribute)
{
    return (struct ZirAttribute*)fl_hashtable_add(attr_map->map, attribute->name, attribute);
}

static inline const char** zir_attribute_map_keys(struct ZirAttributeMap *attr_map)
{
    return fl_hashtable_keys(attr_map->map);
}

static inline struct ZirAttribute** zir_attribute_map_values(struct ZirAttributeMap *attr_map)
{
    return fl_hashtable_values(attr_map->map);
}

static inline struct ZirAttribute* zir_attribute_map_get(struct ZirAttributeMap *attr_map, const char *attribute_name)
{
    return (struct ZirAttribute*)fl_hashtable_get(attr_map->map, attribute_name);
}

static inline size_t zir_attribute_map_length(struct ZirAttributeMap *attr_map)
{
    return fl_hashtable_length(attr_map->map);
}

static inline bool zir_attribute_map_has_key(struct ZirAttributeMap *attr_map, const char *attribute_name)
{
    return fl_hashtable_has_key(attr_map->map, attribute_name);
}

#endif /* ZIR_ATTRIBUTE_MAP_H */
