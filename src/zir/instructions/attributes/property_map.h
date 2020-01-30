#ifndef ZIR_PROPERTY_MAP_H
#define ZIR_PROPERTY_MAP_H

#include <fllib.h>
#include "property.h"

struct ZirPropertyMap {
    FlHashtable map;
};

static inline struct ZirPropertyMap zir_property_map_new()
{
    return (struct ZirPropertyMap) {
        .map = fl_hashtable_new_args((struct FlHashtableArgs) {
            .hash_function = fl_hashtable_hash_string,
            .key_allocator = fl_container_allocator_string,
            .key_comparer = fl_container_equals_string,
            .key_cleaner = fl_container_cleaner_pointer,
            .value_cleaner = (FlContainerCleanupFunction) zir_property_free,
            .value_allocator = NULL
        })
    };
}

static inline void zir_property_map_free(struct ZirPropertyMap *property_map)
{
    if (property_map->map)
        fl_hashtable_free(property_map->map);
}

static inline struct ZirProperty* zir_property_map_add(struct ZirPropertyMap *property_map, struct ZirProperty *property)
{
    return (struct ZirProperty*)fl_hashtable_add(property_map->map, property->name, property);
}

static inline const char** zir_property_map_keys(struct ZirPropertyMap *property_map)
{
    return fl_hashtable_keys(property_map->map);
}

static inline struct ZirProperty** zir_property_map_values(struct ZirPropertyMap *property_map)
{
    return fl_hashtable_values(property_map->map);
}

static inline struct ZirProperty* zir_property_map_get(struct ZirPropertyMap *property_map, const char *property_name)
{
    return (struct ZirProperty*)fl_hashtable_get(property_map->map, property_name);
}

static inline size_t zir_property_map_length(struct ZirPropertyMap *property_map)
{
    return fl_hashtable_length(property_map->map);
}

static inline bool zir_property_map_has_key(struct ZirPropertyMap *property_map, const char *property_name)
{
    return fl_hashtable_has_key(property_map->map, property_name);
}

#endif /* ZIR_PROPERTY_MAP_H */
