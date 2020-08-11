#ifndef ZIR_ATTRIBUTE_MAP_H
#define ZIR_ATTRIBUTE_MAP_H


#include "attribute.h"

typedef FlHashtable ZirAttributeMap;

static inline ZirAttributeMap* zir_attribute_map_new(void)
{
    return (ZirAttributeMap*) fl_hashtable_new_args((struct FlHashtableArgs) { 
        .hash_function = fl_hashtable_hash_string, 
        .key_allocator = fl_container_allocator_string,
        .key_comparer = fl_container_equals_string,
        .key_cleaner = fl_container_cleaner_pointer,   
        .value_cleaner = (FlContainerCleanupFn) zir_attribute_free,  
        .value_allocator = NULL
    });
}

static inline void zir_attribute_map_free(ZirAttributeMap *attr_map)
{ 
    if (attr_map)
        fl_hashtable_free(attr_map);
}

static inline ZirAttribute* zir_attribute_map_add(ZirAttributeMap *attr_map, ZirAttribute *attribute)
{
    return (ZirAttribute*) fl_hashtable_add(attr_map, attribute->name, attribute);
}

static inline const char** zir_attribute_map_keys(ZirAttributeMap *attr_map)
{
    return fl_hashtable_keys(attr_map);
}

static inline ZirAttribute** zir_attribute_map_values(ZirAttributeMap *attr_map)
{
    return (ZirAttribute**) fl_hashtable_values(attr_map);
}

static inline ZirAttribute* zir_attribute_map_get(ZirAttributeMap *attr_map, const char *attribute_name)
{
    return (ZirAttribute*) fl_hashtable_get(attr_map, attribute_name);
}

static inline size_t zir_attribute_map_length(ZirAttributeMap *attr_map)
{
    return fl_hashtable_length(attr_map);
}

static inline bool zir_attribute_map_has_key(ZirAttributeMap *attr_map, const char *attribute_name)
{
    return fl_hashtable_has_key(attr_map, attribute_name);
}

#endif /* ZIR_ATTRIBUTE_MAP_H */
