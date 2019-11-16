#ifndef ZENIT_IR_PROPERTY_H
#define ZENIT_IR_PROPERTY_H

#include <fllib.h>
#include "operand.h"

struct ZenitIrProperty {
    char *name;
    struct ZenitIrOperand value;
};

void zenit_ir_property_free(struct ZenitIrProperty *property);

struct ZenitIrPropertyMap {
    FlHashtable map;
};

inline struct ZenitIrPropertyMap zenit_ir_property_map_new()
{
    return (struct ZenitIrPropertyMap) {
        .map = fl_hashtable_new_args((struct FlHashtableArgs) {
            .hash_function = fl_hashtable_hash_string,
            .key_allocator = fl_container_allocator_string,
            .key_comparer = fl_container_equals_string,
            .key_cleaner = fl_container_cleaner_pointer,
            .value_cleaner = (FlContainerCleanupFunction)zenit_ir_property_free,
            .value_allocator = NULL
        })
    };
}

inline void zenit_ir_property_map_free(struct ZenitIrPropertyMap *property_map)
{
    fl_hashtable_free(property_map->map);
}

inline struct ZenitIrProperty* zenit_ir_property_map_add(struct ZenitIrPropertyMap *property_map, struct ZenitIrProperty *property)
{
    return (struct ZenitIrProperty*)fl_hashtable_add(property_map->map, property->name, property);
}

inline const char** zenit_ir_property_map_keys(struct ZenitIrPropertyMap *property_map)
{
    return fl_hashtable_keys(property_map->map);
}

inline struct ZenitIrProperty** zenit_ir_property_map_values(struct ZenitIrPropertyMap *property_map)
{
    return fl_hashtable_values(property_map->map);
}

inline struct ZenitIrProperty* zenit_ir_property_map_get(struct ZenitIrPropertyMap *property_map, const char *property_name)
{
    return (struct ZenitIrProperty*)fl_hashtable_get(property_map->map, property_name);
}

inline size_t zenit_ir_property_map_length(struct ZenitIrPropertyMap *property_map)
{
    return fl_hashtable_length(property_map->map);
}

inline bool zenit_ir_property_map_has_key(struct ZenitIrPropertyMap *property_map, const char *property_name)
{
    return fl_hashtable_has_key(property_map->map, property_name);
}

#endif /* ZENIT_IR_PROPERTY_H */
