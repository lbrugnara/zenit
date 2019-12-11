#include <fllib.h>
#include <stdlib.h>
#include "type.h"

#define TYPE_KEY_FORMAT "[n:%s][t:%d][a:%d][e:%zu][r:%d]", typeinfo->name, typeinfo->type, typeinfo->is_array, typeinfo->elements, typeinfo->is_ref

/*
 * Struct: TypeMapping
 *  Internal structure to keep a lookup list between type's string
 *  representation and its corresponding <enum ZirType> value
 */
static struct TypeMapping {
    const char *string;
    enum ZirType type;
} type_mappings[] = {
    { "<none>", ZIR_TYPE_NONE },
    { "uint8",  ZIR_TYPE_UINT8 },
    { "uint16", ZIR_TYPE_UINT16 },
};

/*
 * Variable: type_string_mapping_pool
 *  Because many <struct ZirTypeInfo> objects that are equals
 *  map to the same string representation, we use a hashtable
 *  to reuse those strings between them.
 *  The hashtable uses <struct ZirTypeInfo> as key elements and 
 *  saves string for each value
 */
static FlHashtable type_string_mapping_pool = NULL;

/*
 * Function: hash_type
 *  Creates a hash from a <struct ZirTypeInfo> object
 */
static unsigned long hash_type(const FlByte *key)
{
    struct ZirTypeInfo *typeinfo = (struct ZirTypeInfo*)key;

    size_t length = snprintf(NULL, 0, TYPE_KEY_FORMAT);
    char *buffer = fl_cstring_new(length);
    snprintf(buffer, length + 1, TYPE_KEY_FORMAT);
    buffer[length] = '\0';

    unsigned long hash = 5381;
    FlByte c;

    for (size_t i=0; i < length; i++)
        hash = ((hash << 5) + hash) + buffer[i];

    fl_cstring_free(buffer);

    return hash;
}

/*
 * Function: alloc_type_key
 *  Allocates a <struct ZirTypeInfo> object into the *dest* pointer
 */
static void alloc_type_key(FlByte **dest, const FlByte *src)
{
    *dest = fl_malloc(sizeof(struct ZirTypeInfo));
    memcpy(*dest, src, sizeof(struct ZirTypeInfo));
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
 *  Initializes the pool of mappings between <struct ZirTypeInfo> objects
 *  and strings
 */
static inline void init_pool(void)
{
    if (type_string_mapping_pool)
        return;

    type_string_mapping_pool = fl_hashtable_new_args((struct FlHashtableArgs) {
        .hash_function = hash_type,
        .key_allocator = alloc_type_key,
        .key_comparer = (FlContainerEqualsFunction)zir_type_equals,
        .key_cleaner = fl_container_cleaner_pointer,
        .value_cleaner = (FlContainerCleanupFunction)fl_cstring_free,
        .value_allocator = NULL
    });

    atexit(free_pool);
}

/*
 * Function: zir_type_to_string
 *  This function has the added complexity of taking into account if 
 *  the type is an array and its size, therefore we need to use a heap
 *  allocated string, but we benefit from the <type_string_mapping_pool> variable
 *  to reuse strings.
 */
const char* zir_type_to_string(const struct ZirTypeInfo *typeinfo)
{
    // We need to initialize the pool first
    if (!type_string_mapping_pool)
        init_pool();

    // If we already processed the string representation of the <struct ZirTypeInfo> object
    // we return it
    if (fl_hashtable_has_key(type_string_mapping_pool, typeinfo))
        return fl_hashtable_get(type_string_mapping_pool, typeinfo);

    // If the base type is a struct type, we use the struct type's name
    const char *base_type = ZIR_TYPE_CUSTOM == typeinfo->type ? typeinfo->name : NULL;

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

    // We allocate memory for the string representation of this <struct ZirTypeInfo> object
    char *string_value = fl_cstring_new(0);
    
    // Add the arrray information if needed
    if (typeinfo->is_array)
    {
        fl_cstring_append(&string_value, "[");
        fl_cstring_vappend(&string_value, "%zu", typeinfo->elements);
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
 * Function: zir_type_to_base_string
 *  The base string representation is easier than the <zir_type_to_string>
 *  function because we don't care about array information
 */
const char* zir_type_to_base_string(const struct ZirTypeInfo *typeinfo)
{
    if (ZIR_TYPE_CUSTOM == typeinfo->type)
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
 * Function: zir_type_copy
 *  We safely assign memory if needed because the owner of the
 *  <struct ZirTypeInfo> is in charge of releasing it
 */
void zir_type_copy(struct ZirTypeInfo *dest_type, struct ZirTypeInfo *src_type)
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
 * Function: zir_type_equals
 *  This basic function compares types, names, reference and array information
 */
bool zir_type_equals(struct ZirTypeInfo *type_a, struct ZirTypeInfo *type_b)
{
    if (type_a == NULL || type_b == NULL)
        return type_a == type_b;

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

size_t zir_type_size(struct ZirTypeInfo *type)
{
    if (!type)
        return 0;

    size_t element_size = 0;

    switch (type->type)
    {
        case ZIR_TYPE_UINT8:
            element_size = 1; // 1 Byte
            break;

        case ZIR_TYPE_UINT16:
            element_size = 2; // 2 bytes
            break;

        default:
            break;
    }

    if (fl_std_umult_wrap(element_size, type->elements, SIZE_MAX))
        return 0;

    return element_size * type->elements;
}

void zir_type_free(struct ZirTypeInfo *typeinfo)
{
    if (typeinfo->name)
        fl_cstring_free(typeinfo->name);
}
