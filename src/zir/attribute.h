#ifndef ZENIT_IR_ATTRIBUTE_H
#define ZENIT_IR_ATTRIBUTE_H

#include <fllib.h>
#include "property.h"

struct ZenitIrAttribute {
    char *name;
    struct ZenitIrPropertyMap properties;
};

void zenit_ir_attribute_free(struct ZenitIrAttribute *attribute);

struct ZenitIrAttributeMap {
    FlHashtable map;
};


inline struct ZenitIrAttributeMap zenit_ir_attribute_map_new()
{
    return (struct ZenitIrAttributeMap) {
        .map = fl_hashtable_new_args((struct FlHashtableArgs) {
            .hash_function = fl_hashtable_hash_string,
            .key_allocator = fl_container_allocator_string,
            .key_comparer = fl_container_equals_string,
            .key_cleaner = fl_container_cleaner_pointer,
            .value_cleaner = (FlContainerCleanupFunction)zenit_ir_attribute_free,
            .value_allocator = NULL
        })
    };
}

inline void zenit_ir_attribute_map_free(struct ZenitIrAttributeMap *attr_map)
{
    fl_hashtable_free(attr_map->map);
}

inline struct ZenitIrAttribute* zenit_ir_attribute_map_add(struct ZenitIrAttributeMap *attr_map, struct ZenitIrAttribute *attribute)
{
    return (struct ZenitIrAttribute*)fl_hashtable_add(attr_map->map, attribute->name, attribute);
}

inline const char** zenit_ir_attribute_map_keys(struct ZenitIrAttributeMap *attr_map)
{
    return fl_hashtable_keys(attr_map->map);
}

inline struct ZenitIrAttribute** zenit_ir_attribute_map_values(struct ZenitIrAttributeMap *attr_map)
{
    return fl_hashtable_values(attr_map->map);
}

inline struct ZenitIrAttribute* zenit_ir_attribute_map_get(struct ZenitIrAttributeMap *attr_map, const char *attribute_name)
{
    return (struct ZenitIrAttribute*)fl_hashtable_get(attr_map->map, attribute_name);
}

inline size_t zenit_ir_attribute_map_length(struct ZenitIrAttributeMap *attr_map)
{
    return fl_hashtable_length(attr_map->map);
}

inline bool zenit_ir_attribute_map_has_key(struct ZenitIrAttributeMap *attr_map, const char *attribute_name)
{
    return fl_hashtable_has_key(attr_map->map, attribute_name);
}

#endif /* ZENIT_IR_ATTRIBUTE_H */
