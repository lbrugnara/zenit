#ifndef ZIR_PROPERTY_MAP_H
#define ZIR_PROPERTY_MAP_H

#include <fllib/containers/Hashtable.h>
#include "property.h"

typedef FlHashtable ZirPropertyMap;

static inline ZirPropertyMap* zir_property_map_new(void)
{
    return (ZirPropertyMap*) fl_hashtable_new_args((struct FlHashtableArgs) {
        .hash_function = fl_hashtable_hash_string,
        .key_allocator = fl_container_allocator_string,
        .key_comparer = fl_container_equals_string,
        .key_cleaner = fl_container_cleaner_pointer,  
        .value_cleaner = (FlContainerCleanupFn) zir_property_free,  
        .value_allocator = NULL
    });
}

static inline void zir_property_map_free(ZirPropertyMap *property_map)
{ 
    if (property_map)
        fl_hashtable_free(property_map);
}

static inline ZirProperty* zir_property_map_add(ZirPropertyMap *property_map, ZirProperty *property)
{
    return (ZirProperty*) fl_hashtable_add(property_map, property->name, property);
}

static inline const char** zir_property_map_keys(ZirPropertyMap *property_map)
{
    return fl_hashtable_keys(property_map);
}

static inline ZirProperty** zir_property_map_values(ZirPropertyMap *property_map)
{
    return (ZirProperty**) fl_hashtable_values(property_map);
}

static inline ZirProperty* zir_property_map_get(ZirPropertyMap *property_map, const char *property_name)
{
    return (ZirProperty*) fl_hashtable_get(property_map, property_name);
}

static inline size_t zir_property_map_length(ZirPropertyMap *property_map)
{
    return fl_hashtable_length(property_map);
}

static inline bool zir_property_map_has_key(ZirPropertyMap *property_map, const char *property_name)
{
    return fl_hashtable_has_key(property_map, property_name);
}

#endif /* ZIR_PROPERTY_MAP_H */
