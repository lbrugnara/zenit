#ifndef ZIR_PROPERTY_MAP_H
#define ZIR_PROPERTY_MAP_H

#include <fllib/containers/Hashtable.h>
#include "property.h"

typedef FlHashtable ZirPropertyMap;

#define zir_property_map_new()  \
    ((ZirPropertyMap*) fl_hashtable_new_args((struct FlHashtableArgs) { \
            .hash_function = fl_hashtable_hash_string,  \
            .key_allocator = fl_container_allocator_string, \
            .key_comparer = fl_container_equals_string, \
            .key_cleaner = fl_container_cleaner_pointer,    \
            .value_cleaner = (FlContainerCleanupFn) zir_property_free,    \
            .value_allocator = NULL \
        }))

#define zir_property_map_free(property_map) do { if (property_map) fl_hashtable_free(property_map); } while (0)

#define zir_property_map_add(property_map, property) ((ZirProperty*) fl_hashtable_add(property_map, property->name, property))

#define zir_property_map_keys(property_map) fl_hashtable_keys(property_map)

#define zir_property_map_values(property_map) ((ZirProperty**) fl_hashtable_values(property_map))

#define zir_property_map_get(property_map, property_name) ((ZirProperty*) fl_hashtable_get(property_map, property_name))

#define zir_property_map_length(property_map) fl_hashtable_length(property_map)

#define zir_property_map_has_key(property_map, property_name) fl_hashtable_has_key(property_map, property_name)

#endif /* ZIR_PROPERTY_MAP_H */
