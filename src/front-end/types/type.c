#include <fllib.h>
#include <stdlib.h>
#include "type.h"
#include "system.h"

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

static char* get_type_key(struct ZenitTypeInfo *typeinfo)
{
    if (typeinfo->type == ZENIT_TYPE_STRUCT)
    {
        struct ZenitStructTypeInfo *sti = (struct ZenitStructTypeInfo*) typeinfo;
        // FIXME: Once the struct members are implemented, fix the key
        return fl_cstring_vdup("[struct][n:%s][m:%zu]", sti->name, sti->members ? fl_array_length(sti->members) : 0);
    }


    if (typeinfo->type == ZENIT_TYPE_REFERENCE)
    {
        struct ZenitReferenceTypeInfo *rti = (struct ZenitReferenceTypeInfo*) typeinfo;

        char *element_key = get_type_key(rti->element);
        char *key = fl_cstring_vdup("[ref][e:%s]", element_key);
        fl_cstring_free(element_key);

        return key;
    }
    
    if (typeinfo->type == ZENIT_TYPE_ARRAY)
    {
        struct ZenitArrayTypeInfo *ati = (struct ZenitArrayTypeInfo*) typeinfo;

        size_t length = fl_array_length(ati->members);
        char *key = fl_cstring_vdup("[array][n:%zu]", length);

        for (size_t i=0; i < length; i++)
        {
            char *element_key = get_type_key(ati->members[i]);
            fl_cstring_append(&key, element_key);
            fl_cstring_free(element_key);
        }

        return key;
    }

    if (zenit_type_is_primitive(typeinfo->type))
        return fl_cstring_vdup("[primitive][t:%s]", zenit_type_to_base_string(typeinfo));
    
    return NULL;
}

/*
 * Function: hash_type
 *  Creates a hash from a <struct ZenitTypeInfo> object
 */
static unsigned long hash_type(const FlByte *key)
{
    struct ZenitTypeInfo *typeinfo = (struct ZenitTypeInfo*)key;

    char *type_key = get_type_key(typeinfo);

    if (type_key == NULL)
        return 0;

    unsigned long hash = 5381;
    FlByte c;

    for (size_t i=0; i < strlen(type_key); i++)
        hash = ((hash << 5) + hash) + type_key[i];

    fl_cstring_free(type_key);

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

const char* zenit_type_name(enum ZenitType type)
{
    for (size_t i=0; i < sizeof(type_mappings) / sizeof(type_mappings[0]); i++)
    {
        struct TypeMapping mapping = type_mappings[i];
        if (mapping.type == type)
        {
            return mapping.string;
        }
    }

    return "<unknown>";
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
    // FIXME: We can create a function for each ZENIT_TYPE_* object, that way complex
    // types like "struct" and "arrays" can recursively add information. The current
    // implementation is compatible with the previous version of the type system, so
    // it allows us to quickly get on track

    // We need to initialize the pool first
    if (!type_string_mapping_pool)
        init_pool();

    // If we already processed the string representation of the <struct ZenitTypeInfo> object
    // we return it
    if (fl_hashtable_has_key(type_string_mapping_pool, typeinfo))
        return fl_hashtable_get(type_string_mapping_pool, typeinfo);

    // If the base type is a struct type, we use the struct type's name
    const char *base_type = ZENIT_TYPE_STRUCT == typeinfo->type ? ((struct ZenitStructTypeInfo*) typeinfo)->name : NULL;

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
    if (ZENIT_TYPE_ARRAY == typeinfo->type)
    {
        fl_cstring_append(&string_value, "[");
        char tmp_string[20];
        snprintf(tmp_string, 20, "%zu", fl_array_length(((struct ZenitStructTypeInfo*) typeinfo)->members));
        fl_cstring_append(&string_value, tmp_string);
        fl_cstring_append(&string_value, "]");
    }

    if (ZENIT_TYPE_REFERENCE == typeinfo->type)
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
        return ((struct ZenitStructTypeInfo*) typeinfo)->name;

    for (size_t i=0; i < sizeof(type_mappings) / sizeof(type_mappings[0]); i++)
    {
        struct TypeMapping mapping = type_mappings[i];
        if (mapping.type == typeinfo->type)
            return mapping.string;
    }

    return "<unknown>";
}

struct ZenitTypeInfo* zenit_type_copy(struct ZenitTypeInfo *src_type)
{
    if (!src_type)
        return NULL;

    if (src_type->type == ZENIT_TYPE_STRUCT)
    {
        // FIXME: Once the members are implemented we need to copy them too
        return (struct ZenitTypeInfo*) zenit_type_struct_new(((struct ZenitStructTypeInfo*) src_type)->name);
    }

    if (src_type->type == ZENIT_TYPE_REFERENCE)
        return (struct ZenitTypeInfo*) zenit_type_reference_new(((struct ZenitReferenceTypeInfo*) src_type)->element);


    if (src_type->type == ZENIT_TYPE_ARRAY)
    {
        struct ZenitArrayTypeInfo *src_array = (struct ZenitArrayTypeInfo*) src_type;
        
        struct ZenitArrayTypeInfo *dst_array = zenit_type_array_new();
        dst_array->members = fl_array_combine(dst_array->members, src_array->members);
        
        return (struct ZenitTypeInfo*) dst_array;
    }

    if (zenit_type_is_primitive(src_type->type))
        return (struct ZenitTypeInfo*) zenit_type_primitive_new(src_type->type);

    return NULL;
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

    if (type_a->type == ZENIT_TYPE_STRUCT)
    {
        // FIXME: Once the members are implemented we need to copy them too
        struct ZenitStructTypeInfo *s_type_a = (struct ZenitStructTypeInfo*) type_a;
        struct ZenitStructTypeInfo *s_type_b = (struct ZenitStructTypeInfo*) type_b;
        
        return flm_cstring_equals(s_type_a->name, s_type_b->name);
    }

    if (type_a->type == ZENIT_TYPE_REFERENCE)
    {
        struct ZenitReferenceTypeInfo *r_type_a = (struct ZenitReferenceTypeInfo*) type_a;
        struct ZenitReferenceTypeInfo *r_type_b = (struct ZenitReferenceTypeInfo*) type_b;

        return zenit_type_equals(r_type_a->element, r_type_b->element);
    }


    if (type_a->type == ZENIT_TYPE_ARRAY)
    {
        struct ZenitArrayTypeInfo *a_type_a = (struct ZenitArrayTypeInfo*) type_a;
        struct ZenitArrayTypeInfo *a_type_b = (struct ZenitArrayTypeInfo*) type_b;
        
        if (fl_array_length(a_type_a->members) != fl_array_length(a_type_b->members))
            return false;

        for (size_t i=0; i < fl_array_length(a_type_a->members); i++)
            if (!zenit_type_equals(a_type_a->members[i], a_type_b->members[i]))
                return false;
        
        return true;
    }

    return false;
}

struct ZenitTypeInfo* zenit_type_unify(struct ZenitTypeInfo *type_a, struct ZenitTypeInfo *type_b)
{
    if (type_a->type == type_b->type && type_a->type == ZENIT_TYPE_NONE)
        return NULL;

    if (type_a->type == ZENIT_TYPE_NONE)
        return zenit_type_copy(type_b);

    if (type_b->type == ZENIT_TYPE_NONE)
        return zenit_type_copy(type_a);

    if (zenit_type_is_primitive(type_a->type) && zenit_type_is_primitive(type_b->type))
    {
        bool a_uint = type_a->type >= ZENIT_TYPE_UINT8 && type_a->type <= ZENIT_TYPE_UINT16;
        bool b_uint = type_b->type >= ZENIT_TYPE_UINT8 && type_b->type <= ZENIT_TYPE_UINT16;
        
        if (a_uint && b_uint)
        {
            if (type_a->type > type_b->type)
                return zenit_type_copy(type_a);

            return zenit_type_copy(type_b);
        }
    }

    // FIXME: Handle complex types

    return NULL;
}

bool zenit_type_can_assign(struct ZenitTypeInfo *target_type, struct ZenitTypeInfo *value_type)
{
    if (zenit_type_equals(target_type, value_type))
        return true;

    if (target_type->type == ZENIT_TYPE_NONE && value_type->type == ZENIT_TYPE_NONE)
        return false;

    if (target_type->type == ZENIT_TYPE_NONE)
        return true;

    if (value_type->type == ZENIT_TYPE_NONE)
        return false;

    if (zenit_type_is_primitive(target_type->type) && zenit_type_is_primitive(value_type->type))
    {
        bool a_uint = target_type->type >= ZENIT_TYPE_UINT8 && target_type->type <= ZENIT_TYPE_UINT16;
        bool b_uint = value_type->type >= ZENIT_TYPE_UINT8 && value_type->type <= ZENIT_TYPE_UINT16;
        
        if (a_uint && b_uint)
        {
            if (target_type->type >= value_type->type)
                return true;
        }
    }

    return false;
}


bool zenit_type_can_cast(struct ZenitTypeInfo *target_type, struct ZenitTypeInfo *cast_type)
{
    // If we can assign from the target type to the cast type, it means
    // the types are compatible, and we could proceed with the down casting
    // by truncating the cast_type to a target_type
    if (zenit_type_can_assign(cast_type, target_type))
        return true;

    // We can't cast things we don't know
    if (target_type->type == ZENIT_TYPE_NONE || cast_type->type == ZENIT_TYPE_NONE)
        return false;

    bool target_uint = target_type->type >= ZENIT_TYPE_UINT8 && target_type->type <= ZENIT_TYPE_UINT16;
    bool cast_uint = cast_type->type >= ZENIT_TYPE_UINT8 && cast_type->type <= ZENIT_TYPE_UINT16;

    // We can cast a reference to an unsigned integer
    if (cast_type->type == ZENIT_TYPE_REFERENCE && target_uint)
        return true;

    if (target_type->type == ZENIT_TYPE_REFERENCE && target_type->type != cast_type->type)
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
    if (typeinfo->type == ZENIT_TYPE_STRUCT)
    {
        zenit_type_struct_free((struct ZenitStructTypeInfo*) typeinfo);
        return;
    }

    if (typeinfo->type == ZENIT_TYPE_REFERENCE)
    {
        zenit_type_reference_free((struct ZenitReferenceTypeInfo*) typeinfo);
        return;
    }


    if (typeinfo->type == ZENIT_TYPE_ARRAY)
    {
        zenit_type_array_free((struct ZenitArrayTypeInfo*) typeinfo);
        return;
    }

    if (zenit_type_is_primitive(typeinfo->type))
    {
        zenit_type_primitive_free((struct ZenitPrimitiveTypeInfo*) typeinfo);
        return;
    }


    fl_free(typeinfo);
}
