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

        size_t buffer_length = snprintf(NULL, 0, "[array][n:%zu]", length);
        char *key = fl_cstring_new(buffer_length);
        snprintf(key, buffer_length + 1, "[array][n:%zu]", length);
        key[buffer_length] = '\0';

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
 * Function: zenit_type_hash
 *  Creates a hash from a <struct ZenitTypeInfo> object
 */
unsigned long zenit_type_hash(struct ZenitTypeInfo *typeinfo)
{
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
const char* zenit_type_to_string(struct ZenitTypeInfo *typeinfo)
{
    if (typeinfo == NULL)
        return NULL;

    unsigned long type_hash = zenit_type_hash(typeinfo);

    if (typeinfo->to_string.value == NULL)
    {
        // First call, initialize the value
        typeinfo->to_string.value = fl_cstring_new(0);
    }
    else if (type_hash == typeinfo->to_string.version)
    {
        // If the type information didn't change, just return the 
        // current value
        return typeinfo->to_string.value;
    }

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
        struct ZenitArrayTypeInfo *array_type = (struct ZenitArrayTypeInfo*) typeinfo;

        size_t members_count = 0;
        if (array_type->member_type != NULL)
        {
            base_type = zenit_type_to_string(array_type->member_type);
            members_count = array_type->length;
        }
        else
        {
            base_type = "<unknown>";
            members_count = 0;
        }

        fl_cstring_append(&string_value, "[");
        char tmp_string[20];
        snprintf(tmp_string, 20, "%zu", members_count);
        fl_cstring_append(&string_value, tmp_string);
        fl_cstring_append(&string_value, "]");
    }

    if (ZENIT_TYPE_REFERENCE == typeinfo->type)
    {
        fl_cstring_append(&string_value, "&");
        struct ZenitReferenceTypeInfo *ref_type = (struct ZenitReferenceTypeInfo*) typeinfo;
        base_type = zenit_type_to_string(ref_type->element);
    }

    // Add the base type
    fl_cstring_append(&string_value, base_type);

    // Update the string representation
    typeinfo->to_string.version = type_hash;
    typeinfo->to_string.value = fl_cstring_replace_realloc(typeinfo->to_string.value, typeinfo->to_string.value, string_value);

    fl_cstring_free(string_value);

    return typeinfo->to_string.value;
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
    {
        struct ZenitTypeInfo *referred_type = zenit_type_copy(((struct ZenitReferenceTypeInfo*) src_type)->element);
        return (struct ZenitTypeInfo*) zenit_type_reference_new(referred_type);
    }


    if (src_type->type == ZENIT_TYPE_ARRAY)
    {
        struct ZenitArrayTypeInfo *src_array = (struct ZenitArrayTypeInfo*) src_type;
        
        struct ZenitArrayTypeInfo *dst_array = zenit_type_array_new();

        for (size_t i=0; i < fl_array_length(src_array->members); i++)
            zenit_type_array_add_member(dst_array, zenit_type_copy(src_array->members[i]));
        
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

    if (type_a->type == ZENIT_TYPE_STRUCT)
    {
        if (type_b->type != ZENIT_TYPE_STRUCT)
            return false;

        // FIXME: Once the members are implemented we need to copy them too
        struct ZenitStructTypeInfo *s_type_a = (struct ZenitStructTypeInfo*) type_a;
        struct ZenitStructTypeInfo *s_type_b = (struct ZenitStructTypeInfo*) type_b;
        
        if (s_type_a->name == NULL || s_type_b->name == NULL)
            return s_type_a->name == s_type_b->name;

        return flm_cstring_equals(s_type_a->name, s_type_b->name);
    }

    if (type_a->type == ZENIT_TYPE_REFERENCE)
    {
        if (type_b->type != ZENIT_TYPE_REFERENCE)
            return false;

        struct ZenitReferenceTypeInfo *r_type_a = (struct ZenitReferenceTypeInfo*) type_a;
        struct ZenitReferenceTypeInfo *r_type_b = (struct ZenitReferenceTypeInfo*) type_b;

        return zenit_type_equals(r_type_a->element, r_type_b->element);
    }

    if (type_a->type == ZENIT_TYPE_ARRAY)
    {
        if (type_b->type != ZENIT_TYPE_ARRAY)
            return false;

        struct ZenitArrayTypeInfo *a_type_a = (struct ZenitArrayTypeInfo*) type_a;
        struct ZenitArrayTypeInfo *a_type_b = (struct ZenitArrayTypeInfo*) type_b;

        return a_type_a->length == a_type_b->length && zenit_type_equals(a_type_a->member_type, a_type_b->member_type);
    }

    if (zenit_type_is_primitive(type_a->type) && zenit_type_is_primitive(type_b->type))
        return type_a->type == type_b->type;

    return false;
}

struct ZenitTypeInfo* zenit_type_unify(struct ZenitTypeInfo *type_a, struct ZenitTypeInfo *type_b)
{
    // FIXME: This is a pretty bad algorithm for unifying ... but it works for uints, so...

    if (zenit_type_equals(type_a, type_b))
        return type_a;

    if (type_a->type == ZENIT_TYPE_NONE && type_b->type == ZENIT_TYPE_NONE)
        return NULL;

    if (type_a->type == ZENIT_TYPE_NONE)
        return type_b;

    if (type_b->type == ZENIT_TYPE_NONE)
        return type_a;

    if (zenit_type_is_primitive(type_a->type) && zenit_type_is_primitive(type_b->type))
    {
        bool a_uint = type_a->type >= ZENIT_TYPE_UINT8 && type_a->type <= ZENIT_TYPE_UINT16;
        bool b_uint = type_b->type >= ZENIT_TYPE_UINT8 && type_b->type <= ZENIT_TYPE_UINT16;
        
        if (a_uint && b_uint)
        {
            if (type_a->type > type_b->type)
                return type_a;

            return type_b;
        }
    }

    if (type_a->type == ZENIT_TYPE_STRUCT)
    {
        if (type_b->type != ZENIT_TYPE_STRUCT)
            return NULL;

        // FIXME: Once the members are implemented we need to copy them too
        struct ZenitStructTypeInfo *s_type_a = (struct ZenitStructTypeInfo*) type_a;
        struct ZenitStructTypeInfo *s_type_b = (struct ZenitStructTypeInfo*) type_b;
        
        if (flm_cstring_equals(s_type_a->name, s_type_b->name))
            return type_a;

        return NULL;
    }

    if (type_a->type == ZENIT_TYPE_REFERENCE)
    {
        if (type_b->type != ZENIT_TYPE_REFERENCE)
            return NULL;

        struct ZenitReferenceTypeInfo *r_type_a = (struct ZenitReferenceTypeInfo*) type_a;
        struct ZenitReferenceTypeInfo *r_type_b = (struct ZenitReferenceTypeInfo*) type_b;

        return zenit_type_unify(r_type_a->element, r_type_b->element);
    }

    if (type_a->type == ZENIT_TYPE_ARRAY)
    {
        if (type_b->type != ZENIT_TYPE_ARRAY)
            return NULL;

        struct ZenitArrayTypeInfo *a_type_a = (struct ZenitArrayTypeInfo*) type_a;
        struct ZenitArrayTypeInfo *a_type_b = (struct ZenitArrayTypeInfo*) type_b;

        if (a_type_a->length != a_type_b->length)
            return NULL;

        struct ZenitTypeInfo *unified = zenit_type_unify(a_type_a->member_type, a_type_b->member_type);

        if (unified == NULL)
            return NULL;
        
        // FIXME: We need a new type here, and we should ask the caller to register the type 
        return type_a;
    }

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

    if (target_type->type == ZENIT_TYPE_STRUCT)
    {
        if (value_type->type != ZENIT_TYPE_STRUCT)
            return false;

        // FIXME: Once the members are implemented we need to copy them too
        struct ZenitStructTypeInfo *s_target_type = (struct ZenitStructTypeInfo*) target_type;
        struct ZenitStructTypeInfo *s_value_type = (struct ZenitStructTypeInfo*) value_type;
        
        if (flm_cstring_equals(s_target_type->name, s_value_type->name))
            return true;

        return false;
    }

    if (target_type->type == ZENIT_TYPE_REFERENCE)
    {
        if (value_type->type != ZENIT_TYPE_REFERENCE)
            return false;

        struct ZenitReferenceTypeInfo *r_target_type = (struct ZenitReferenceTypeInfo*) target_type;
        struct ZenitReferenceTypeInfo *r_value_type = (struct ZenitReferenceTypeInfo*) value_type;

        return zenit_type_can_assign(r_target_type->element, r_value_type->element);
    }

    if (target_type->type == ZENIT_TYPE_ARRAY)
    {
        if (value_type->type != ZENIT_TYPE_ARRAY)
            return false;

        struct ZenitArrayTypeInfo *a_target_type = (struct ZenitArrayTypeInfo*) target_type;
        struct ZenitArrayTypeInfo *a_value_type = (struct ZenitArrayTypeInfo*) value_type;

        if (a_target_type->length != a_value_type->length)
            return false;

        // If rhs is an empty array, as long as the target type is not NONE, it is safe to assign
        if (a_target_type->length == 0 && a_value_type->member_type->type == ZENIT_TYPE_NONE)
            return a_target_type->member_type->type != ZENIT_TYPE_NONE;

        return zenit_type_can_assign(a_target_type->member_type, a_value_type->member_type);
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
    if (!typeinfo)
        return;

    if (typeinfo->type == ZENIT_TYPE_STRUCT)
    {
        zenit_type_struct_free((struct ZenitStructTypeInfo*) typeinfo);
        return;
    }
    else if (typeinfo->type == ZENIT_TYPE_REFERENCE)
    {
        zenit_type_reference_free((struct ZenitReferenceTypeInfo*) typeinfo);
        return;
    }
    else if (typeinfo->type == ZENIT_TYPE_ARRAY)
    {
        zenit_type_array_free((struct ZenitArrayTypeInfo*) typeinfo);
        return;
    }
    else if (zenit_type_is_primitive(typeinfo->type))
    {
        zenit_type_primitive_free((struct ZenitPrimitiveTypeInfo*) typeinfo);
        return;
    }
    else if (typeinfo->type == ZENIT_TYPE_NONE)
    {
        zenit_type_none_free(typeinfo);
        return;
    }
}
