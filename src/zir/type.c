#include <fllib.h>
#include <stdlib.h>
#include "type.h"

/*
 * Struct: TypeMapping
 *  Internal structure to keep a lookup list between type's string
 *  representation and its corresponding <enum ZenitIrType> value
 */
static struct TypeMapping {
    const char *string;
    enum ZenitIrType type;
} type_mappings[] = {
    { "<none>", ZENIT_IR_TYPE_NONE },
    { "uint8",  ZENIT_IR_TYPE_UINT8 },
    { "uint16", ZENIT_IR_TYPE_UINT16 },
};

/*
 * Variable: type_string_mapping_pool
 *  Because many <struct ZenitIrTypeInfo> objects that are equals
 *  map to the same string representation, we use a hashtable
 *  to reuse those strings between them.
 *  The hashtable uses <struct ZenitIrTypeInfo> as key elements and 
 *  saves string for each value
 */
static FlHashtable type_string_mapping_pool = NULL;

/*
 * Function: hash_type
 *  Creates a hash from a <struct ZenitIrTypeInfo> object
 */
static unsigned long hash_type(const FlByte *key)
{
    unsigned long hash = 5381;
    FlByte c;

    for (size_t i=0; i < sizeof(struct ZenitIrTypeInfo); i++)
        hash = ((hash << 5) + hash) + key[i];
    return hash;
}

/*
 * Function: alloc_type_key
 *  Allocates a <struct ZenitIrTypeInfo> object into the *dest* pointer
 */
static void alloc_type_key(FlByte **dest, const FlByte *src)
{
    *dest = fl_malloc(sizeof(struct ZenitIrTypeInfo));
    memcpy(*dest, src, sizeof(struct ZenitIrTypeInfo));
}

/*
 * Function: free_pool
 *  Called *atexit* this function frees the memory allocated
 *  by the <type_string_mapping_pool> variable
 */
static void free_pool(void)
{
    fl_hashtable_free(type_string_mapping_pool);
}

/*
 * Function: init_pool
 *  Initializes the pool of mappings between <struct ZenitIrTypeInfo> objects
 *  and strings
 */
static inline void init_pool(void)
{
    if (type_string_mapping_pool)
        return;

    type_string_mapping_pool = fl_hashtable_new_args((struct FlHashtableArgs) {
        .hash_function = hash_type,
        .key_allocator = alloc_type_key,
        .key_comparer = fl_container_equals_string,
        .key_cleaner = fl_container_cleaner_pointer,
        .value_cleaner = (FlContainerCleanupFunction)fl_cstring_free,
        .value_allocator = NULL
    });

    atexit(free_pool);
}

/*
 * Function: zenit_ir_type_to_string
 *  This function has the added complexity of taking into account if 
 *  the type is an array and its size, therefore we need to use a heap
 *  allocated string, but we benefit from the <type_string_mapping_pool> variable
 *  to reuse strings.
 */
const char* zenit_ir_type_to_string(const struct ZenitIrTypeInfo *typeinfo)
{
    // We need to initialize the pool first
    if (!type_string_mapping_pool)
        init_pool();

    // If we already processed the string representation of the <struct ZenitIrTypeInfo> object
    // we return it
    if (fl_hashtable_has_key(type_string_mapping_pool, typeinfo))
        return fl_hashtable_get(type_string_mapping_pool, typeinfo);

    // If the base type is a custom type, we use the custom type's name
    const char *base_type = ZENIT_IR_TYPE_CUSTOM == typeinfo->type ? typeinfo->name : NULL;

    // If it is a native type, we need to lookup its native string representation
    if (base_type == NULL)
    {
        for (size_t i=0; i < sizeof(type_mappings) / sizeof(type_mappings[0]); i++)
        {
            struct TypeMapping mapping = type_mappings[i];
            if (mapping.type == typeinfo->type)
            {
                base_type = mapping.string;
                break;
            }
        }
    }

    // We allocate memory for the string representation of this <struct ZenitIrTypeInfo> object
    char *string_value = fl_cstring_new(0);
    
    // Add the arrray information if needed
    if (typeinfo->is_array)
    {
        fl_cstring_append(&string_value, "[");
        char tmp_string[20];
        snprintf(tmp_string, 20, "%zu", typeinfo->elements);
        fl_cstring_append(&string_value, tmp_string);
        fl_cstring_append(&string_value, "]");
    }

    if (typeinfo->is_ref)
        fl_cstring_append(&string_value, "&");

    // Add the base type
    fl_cstring_append(&string_value, base_type);

    // Save the processed string version
    fl_hashtable_add(type_string_mapping_pool, typeinfo, string_value);

    return string_value;
}

/*
 * Function: zenit_ir_type_to_base_string
 *  The base string representation is easier than the <zenit_ir_type_to_string>
 *  function because we don't care about array information
 */
const char* zenit_ir_type_to_base_string(const struct ZenitIrTypeInfo *typeinfo)
{
    if (ZENIT_IR_TYPE_CUSTOM == typeinfo->type)
        return typeinfo->name;

    for (size_t i=0; i < sizeof(type_mappings) / sizeof(type_mappings[0]); i++)
    {
        struct TypeMapping mapping = type_mappings[i];
        if (mapping.type == typeinfo->type)
            return mapping.string;
    }

    return "<unknown>";
}

/*
 * Function: zenit_ir_type_copy
 *  We safely assign memory if needed because the owner of the
 *  <struct ZenitIrTypeInfo> is in charge of releasing it
 */
void zenit_ir_type_copy(struct ZenitIrTypeInfo *dest_type, struct ZenitIrTypeInfo *src_type)
{
    if (src_type->name)
    {
        if (dest_type->name)
            dest_type->name = fl_cstring_replace(dest_type->name, dest_type->name, src_type->name);
        else
            dest_type->name = fl_cstring_dup(src_type->name);
    }
    dest_type->elements = src_type->elements;
    dest_type->is_array = src_type->is_array;
    dest_type->is_ref = src_type->is_ref;
    dest_type->type = src_type->type;
}

/*
 * Function: zenit_ir_type_equals
 *  This basic function compares types, names, reference and array information
 */
bool zenit_ir_type_equals(struct ZenitIrTypeInfo *type_a, struct ZenitIrTypeInfo *type_b)
{
    if (type_a->type != type_b->type)
        return false;

    if (type_a->name != type_b->name)
        return false;

    if (type_a->is_array != type_b->is_array)
        return false;

    if (type_a->is_ref != type_b->is_ref)
        return false;

    return type_a->elements == type_b->elements;
}
