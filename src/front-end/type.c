#include <fllib.h>
#include <stdlib.h>
#include "type.h"

/*
 * Struct: TypeMapping
 *  Internal structure to keep a lookup list between type's string
 *  representation and its corresponding <CenitType> value
 */
static struct TypeMapping {
    const char *string;
    CenitType type;
} type_mappings[] = {
    { "<none>", CENIT_TYPE_NONE },
    { "uint8", CENIT_TYPE_UINT8 },
};

/*
 * Variable: type_string_mapping_pool
 *  Because many <CenitTypeInfo> objects that are equals
 *  map to the same string representation, we use a hashtable
 *  to reuse those strings between them.
 *  The hashtable uses <CenitTypeInfo> as key elements and 
 *  saves string for each value
 */
static FlHashtable type_string_mapping_pool = NULL;

/*
 * Function: hash_type
 *  Creates a hash from a <CenitTypeInfo> object
 */
static unsigned long hash_type(const FlByte *key)
{
    unsigned long hash = 5381;
    FlByte c;

    for (size_t i=0; i < sizeof(CenitTypeInfo); i++)
        hash = ((hash << 5) + hash) + key[i];
    return hash;
}

/*
 * Function: alloc_type_key
 *  Allocates a <CenitTypeInfo> object into the *dest* pointer
 */
static void alloc_type_key(FlByte **dest, const FlByte *src)
{
    *dest = fl_malloc(sizeof(CenitTypeInfo));
    memcpy(*dest, src, sizeof(CenitTypeInfo));
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
 *  Initializes the pool of mappings between <CenitTypeInfo> objects
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

CenitType cenit_type_to_string_parse(const char *typestr)
{
    for (size_t i=0; i < sizeof(type_mappings) / sizeof(type_mappings[0]); i++)
    {
        struct TypeMapping mapping = type_mappings[i];
        if (flm_cstring_equals(mapping.string, typestr))
            return mapping.type;
    }

    // We simply assume the type is a custom type
    return CENIT_TYPE_CUSTOM;
}

CenitType cenit_type_slice_parse(struct FlSlice *slice)
{
    for (size_t i=0; i < sizeof(type_mappings) / sizeof(type_mappings[0]); i++)
    {
        struct TypeMapping mapping = type_mappings[i];
        if (fl_slice_equals_sequence(slice, (FlByte*)mapping.string, strlen(mapping.string)))
            return mapping.type;
    }

    // We simply assume the type is a custom type
    return CENIT_TYPE_CUSTOM;
}

/*
 * Function: cenit_type_to_string
 *  This function has the added complexity of taking into account if 
 *  the type is an array and its size, therefore we need to use a heap
 *  allocated string, but we benefit from the <type_string_mapping_pool> variable
 *  to reuse strings.
 */
const char* cenit_type_to_string(const CenitTypeInfo *typeinfo)
{
    // We need to initialize the pool first
    if (!type_string_mapping_pool)
        init_pool();

    // If we already processed the string representation of the <CenitTypeInfo> object
    // we return it
    if (fl_hashtable_has_key(type_string_mapping_pool, typeinfo))
        return fl_hashtable_get(type_string_mapping_pool, typeinfo);

    // If the base type is a custom type, we use the custom type's name
    const char *base_type = CENIT_TYPE_CUSTOM == typeinfo->type ? typeinfo->name : NULL;

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

    // We allocate memory for the string representation of this <CenitTypeInfo> object
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

    // Add the base type
    fl_cstring_append(&string_value, base_type);

    // Save the processed string version
    fl_hashtable_add(type_string_mapping_pool, typeinfo, string_value);

    return string_value;
}

/*
 * Function: cenit_type_to_base_string
 *  The base string representation is easier than the <cenit_type_to_string>
 *  function because we don't care about array information
 */
const char* cenit_type_to_base_string(const CenitTypeInfo *typeinfo)
{
    if (CENIT_TYPE_CUSTOM == typeinfo->type)
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
 * Function: cenit_type_copy
 *  We safely assign memory if needed because the owner of the
 *  <CenitTypeInfo> is in charge of releasing it
 */
void cenit_type_copy(CenitTypeInfo *dest_type, CenitTypeInfo *src_type)
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
    dest_type->type = src_type->type;
}

/*
 * Function: cenit_type_equals
 *  This basic function compares types, names, and array information
 */
bool cenit_type_equals(CenitTypeInfo *type_a, CenitTypeInfo *type_b)
{
    if (type_a->type != type_b->type)
        return false;

    if (type_a->name != type_b->name)
        return false;

    if (type_a->is_array != type_b->is_array)
        return false;

    return type_a->elements == type_b->elements;
}
