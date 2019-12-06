#include <fllib.h>
#include <stdlib.h>
#include "type.h"

#define TYPE_KEY_FORMAT "[n:%s][t:%d][a:%d][e:%zu][r:%d]", typeinfo->name, typeinfo->type, typeinfo->is_array, typeinfo->elements, typeinfo->is_ref

/*
 * Struct: TypeMapping
 *  Internal structure to keep a lookup list between type's string
 *  representation and its corresponding <enum ZenitType> value
 */
static struct TypeMapping {
    const char *string;
    enum ZenitType type;
} type_mappings[] = {
    { "<none>",     ZENIT_TYPE_NONE },
    { "uint8",      ZENIT_TYPE_UINT8 },
    { "uint16",     ZENIT_TYPE_UINT16 },
};

/*
 * Variable: type_string_mapping_pool
 *  Because many <struct ZenitTypeInfo> objects that are equals
 *  map to the same string representation, we use a hashtable
 *  to reuse those strings between them.
 *  The hashtable uses <struct ZenitTypeInfo> as key elements and 
 *  saves string for each value
 */
static FlHashtable type_string_mapping_pool = NULL;

/*
 * Function: hash_type
 *  Creates a hash from a <struct ZenitTypeInfo> object
 */
static unsigned long hash_type(const FlByte *key)
{
    struct ZenitTypeInfo *typeinfo = (struct ZenitTypeInfo*)key;

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
 *  Allocates a <struct ZenitTypeInfo> object into the *dest* pointer
 */
static void alloc_type_key(FlByte **dest, const FlByte *src)
{
    *dest = fl_malloc(sizeof(struct ZenitTypeInfo));
    memcpy(*dest, src, sizeof(struct ZenitTypeInfo));
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
 *  Initializes the pool of mappings between <struct ZenitTypeInfo> objects
 *  and strings
 */
static inline void init_pool(void)
{
    if (type_string_mapping_pool)
        return;

    type_string_mapping_pool = fl_hashtable_new_args((struct FlHashtableArgs) {
        .hash_function = hash_type,
        .key_allocator = alloc_type_key,
        .key_comparer = (FlContainerEqualsFunction)zenit_type_equals,
        .key_cleaner = fl_container_cleaner_pointer,
        .value_cleaner = (FlContainerCleanupFunction)fl_cstring_free,
        .value_allocator = NULL
    });

    atexit(free_pool);
}

enum ZenitType zenit_type_string_parse(const char *typestr)
{
    for (size_t i=0; i < sizeof(type_mappings) / sizeof(type_mappings[0]); i++)
    {
        struct TypeMapping mapping = type_mappings[i];
        if (flm_cstring_equals(mapping.string, typestr))
            return mapping.type;
    }

    // We simply assume the type is a struct type
    return ZENIT_TYPE_STRUCT;
}

enum ZenitType zenit_type_slice_parse(struct FlSlice *slice)
{
    for (size_t i=0; i < sizeof(type_mappings) / sizeof(type_mappings[0]); i++)
    {
        struct TypeMapping mapping = type_mappings[i];
        if (fl_slice_equals_sequence(slice, (FlByte*)mapping.string, strlen(mapping.string)))
            return mapping.type;
    }

    // We simply assume the type is a struct type
    return ZENIT_TYPE_STRUCT;
}

/*
 * Function: zenit_type_to_string
 *  This function has the added complexity of taking into account if 
 *  the type is an array and its size, therefore we need to use a heap
 *  allocated string, but we benefit from the <type_string_mapping_pool> variable
 *  to reuse strings.
 */
const char* zenit_type_to_string(const struct ZenitTypeInfo *typeinfo)
{
    // We need to initialize the pool first
    if (!type_string_mapping_pool)
        init_pool();

    // If we already processed the string representation of the <struct ZenitTypeInfo> object
    // we return it
    if (fl_hashtable_has_key(type_string_mapping_pool, typeinfo))
        return fl_hashtable_get(type_string_mapping_pool, typeinfo);

    // If the base type is a struct type, we use the struct type's name
    const char *base_type = ZENIT_TYPE_STRUCT == typeinfo->type ? typeinfo->name : NULL;

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

    // We allocate memory for the string representation of this <struct ZenitTypeInfo> object
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
 * Function: zenit_type_to_base_string
 *  The base string representation is easier than the <zenit_type_to_string>
 *  function because we don't care about array information
 */
const char* zenit_type_to_base_string(const struct ZenitTypeInfo *typeinfo)
{
    if (ZENIT_TYPE_STRUCT == typeinfo->type)
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
 * Function: zenit_type_copy
 *  We safely assign memory if needed because the owner of the
 *  <struct ZenitTypeInfo> is in charge of releasing it
 */
void zenit_type_copy(struct ZenitTypeInfo *dest_type, struct ZenitTypeInfo *src_type)
{
    if (!src_type)
        return;

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
 * Function: zenit_type_equals
 *  This basic function compares types, names, and array information
 */
bool zenit_type_equals(struct ZenitTypeInfo *type_a, struct ZenitTypeInfo *type_b)
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

bool zenit_type_unify(struct ZenitTypeInfo *type_a, struct ZenitTypeInfo *type_b)
{
    if (type_a->type == type_b->type && type_a->type == ZENIT_TYPE_NONE)
        return false;

    if (type_a->type == ZENIT_TYPE_NONE)
    {
        zenit_type_copy(type_a, type_b);
        return true;
    }

    if (type_b->type == ZENIT_TYPE_NONE)
    {
        zenit_type_copy(type_b, type_a);
        return true;
    }

    // FIXME: We should check the array's elements
    if (type_a->is_array != type_b->is_array || type_a->elements != type_b->elements)
        return false;

    if (type_a->is_ref != type_b->is_ref)
        return false;

    if (type_a->name != type_b->name)
        return false;

    bool a_uint = type_a->type >= ZENIT_TYPE_UINT8 && type_a->type <= ZENIT_TYPE_UINT16;
    bool b_uint = type_b->type >= ZENIT_TYPE_UINT8 && type_b->type <= ZENIT_TYPE_UINT16;
    
    if (a_uint && b_uint)
    {
        if (type_a->type > type_b->type)
            zenit_type_copy(type_b, type_a);
        else
            zenit_type_copy(type_a, type_b);
        return true;
    }

    return false;
}

bool zenit_type_can_assign(struct ZenitTypeInfo *target_type, struct ZenitTypeInfo *value_type)
{
    if (zenit_type_equals(target_type, value_type))
        return true;

    if (target_type->type == value_type->type && target_type->type == ZENIT_TYPE_NONE)
        return false;

    if (target_type->type == ZENIT_TYPE_NONE)
        return true;

    if (value_type->type == ZENIT_TYPE_NONE)
        return false;

    // FIXME: We should check the array's elements
    if (target_type->is_array != value_type->is_array || target_type->elements != value_type->elements)
        return false;

    // We can't assign non-references to references nor the other way around
    if (target_type->is_ref != value_type->is_ref)
        return false;

    // FIXME: Custom types
    if (target_type->name != value_type->name)
        return false;

    bool a_uint = target_type->type >= ZENIT_TYPE_UINT8 && target_type->type <= ZENIT_TYPE_UINT16;
    bool b_uint = value_type->type >= ZENIT_TYPE_UINT8 && value_type->type <= ZENIT_TYPE_UINT16;
    
    if (a_uint && b_uint)
    {
        if (target_type->type >= value_type->type)
            return true;
    }

    return false;
}


bool zenit_type_can_cast(struct ZenitTypeInfo *target_type, struct ZenitTypeInfo *cast_type)
{
    // If we can assign from the target type to the cast type, it means
    // the types are compatible, and we could proceed with the dow casting
    // by truncating the cast_type to a target_type
    if (zenit_type_can_assign(cast_type, target_type))
        return true;

    // We can't cast things we don't know
    if (target_type->type == cast_type->type || target_type->type == ZENIT_TYPE_NONE)
        return false;

    // FIXME: We should check the array's elements
    if (target_type->is_array != cast_type->is_array || target_type->elements != cast_type->elements)
        return false;

    // FIXME: Custom types
    if (target_type->name != cast_type->name)
        return false;

    bool target_uint = target_type->type >= ZENIT_TYPE_UINT8 && target_type->type <= ZENIT_TYPE_UINT16;
    bool cast_uint = cast_type->type >= ZENIT_TYPE_UINT8 && cast_type->type <= ZENIT_TYPE_UINT16;

    // We can cast a reference to an unsigned integer
    if (cast_type->is_ref && target_uint)
        return true;

    if (target_type->is_ref != cast_type->is_ref)
        return false;
    
    if (target_uint && cast_uint)
    {
        if (target_type->type >= cast_type->type)
            return true;
    }

    return false;
}

void zenit_type_free(struct ZenitTypeInfo *typeinfo)
{
    if (typeinfo->name)
        fl_cstring_free(typeinfo->name);
}
