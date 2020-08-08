#ifndef ZIR_ATTRIBUTE_MAP_H
#define ZIR_ATTRIBUTE_MAP_H


#include "attribute.h"

typedef FlHashtable ZirAttributeMap;

#define zir_attribute_map_new() \
    ((ZirAttributeMap*) fl_hashtable_new_args((struct FlHashtableArgs) {  \
        .hash_function = fl_hashtable_hash_string,  \
        .key_allocator = fl_container_allocator_string, \
        .key_comparer = fl_container_equals_string, \
        .key_cleaner = fl_container_cleaner_pointer,    \
        .value_cleaner = (FlContainerCleanupFn) zir_attribute_free,   \
        .value_allocator = NULL \
    }))

#define zir_attribute_map_free(attr_map) do { if (attr_map) fl_hashtable_free(attr_map); } while (0)

#define zir_attribute_map_add(attr_map, attribute) ((struct ZirAttribute*) fl_hashtable_add(attr_map, attribute->name, attribute))

#define zir_attribute_map_keys(attr_map) fl_hashtable_keys(attr_map)

#define zir_attribute_map_values(attr_map) ((struct ZirAttribute**) fl_hashtable_values(attr_map))

#define zir_attribute_map_get(attr_map, attribute_name) ((struct ZirAttribute*) fl_hashtable_get(attr_map, attribute_name))

#define zir_attribute_map_length(attr_map) fl_hashtable_length(attr_map)

#define zir_attribute_map_has_key(attr_map, attribute_name) fl_hashtable_has_key(attr_map, attribute_name)

#endif /* ZIR_ATTRIBUTE_MAP_H */
