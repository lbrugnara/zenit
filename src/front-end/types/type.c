#include <fllib.h>
#include <stdlib.h>
#include "type.h"
#include "system.h"

enum ZenitType zenit_type_from_slice(struct FlSlice *slice)
{
    enum ZenitType type = zenit_type_primitive_from_slice(slice);

    return type == ZENIT_TYPE_NONE ? ZENIT_TYPE_STRUCT : type;
}

/*
 * Function: zenit_type_hash
 *  Creates a hash from a <struct ZenitTypeInfo> object
 */
unsigned long zenit_type_hash(struct ZenitTypeInfo *typeinfo)
{
    if (!typeinfo)
        return ULONG_MAX;

    if (typeinfo->type == ZENIT_TYPE_STRUCT)
        return zenit_type_struct_hash((struct ZenitStructTypeInfo*) typeinfo);
    
    if (typeinfo->type == ZENIT_TYPE_REFERENCE)
        return zenit_type_reference_hash((struct ZenitReferenceTypeInfo*) typeinfo);
    
    if (typeinfo->type == ZENIT_TYPE_ARRAY)
        return zenit_type_array_hash((struct ZenitArrayTypeInfo*) typeinfo);
    
    if (zenit_type_is_primitive(typeinfo->type))
        return zenit_type_primitive_hash((struct ZenitPrimitiveTypeInfo*) typeinfo);
    
    if (typeinfo->type == ZENIT_TYPE_NONE)
        return zenit_type_none_hash(typeinfo);

    return 0;
}

char* zenit_type_to_string(struct ZenitTypeInfo *typeinfo)
{
    if (!typeinfo)
        return NULL;

    unsigned long type_hash = zenit_type_hash((struct ZenitTypeInfo*) typeinfo);

    if (type_hash == typeinfo->to_string.version && typeinfo->to_string.value != NULL)
    {
        // If the type information didn't change, just return the 
        // current value
        return typeinfo->to_string.value;
    }

    if (typeinfo->type == ZENIT_TYPE_STRUCT)
        return zenit_type_struct_to_string((struct ZenitStructTypeInfo*) typeinfo);
    
    if (typeinfo->type == ZENIT_TYPE_REFERENCE)
        return zenit_type_reference_to_string((struct ZenitReferenceTypeInfo*) typeinfo);
    
    if (typeinfo->type == ZENIT_TYPE_ARRAY)
        return zenit_type_array_to_string((struct ZenitArrayTypeInfo*) typeinfo);
    
    if (zenit_type_is_primitive(typeinfo->type))
        return zenit_type_primitive_to_string((struct ZenitPrimitiveTypeInfo*) typeinfo);
    
    if (typeinfo->type == ZENIT_TYPE_NONE)
        return "<none>";

    return NULL;
}

struct ZenitTypeInfo* zenit_type_copy(struct ZenitTypeInfo *src_type)
{
    if (!src_type)
        return NULL;

    if (src_type->type == ZENIT_TYPE_STRUCT)
        return (struct ZenitTypeInfo*) zenit_type_struct_copy((struct ZenitStructTypeInfo*) src_type);
    
    if (src_type->type == ZENIT_TYPE_REFERENCE)
        return (struct ZenitTypeInfo*) zenit_type_reference_copy((struct ZenitReferenceTypeInfo*) src_type);
    
    if (src_type->type == ZENIT_TYPE_ARRAY)
        return (struct ZenitTypeInfo*) zenit_type_array_copy((struct ZenitArrayTypeInfo*) src_type);
    
    if (zenit_type_is_primitive(src_type->type))
        return (struct ZenitTypeInfo*) zenit_type_primitive_copy((struct ZenitPrimitiveTypeInfo*) src_type);
    
    if (src_type->type == ZENIT_TYPE_NONE)
        return zenit_type_none_new();

    return NULL;
}

bool zenit_type_equals(struct ZenitTypeInfo *type_a, struct ZenitTypeInfo *type_b)
{
    if (type_a == NULL || type_b == NULL)
        return type_a == type_b;

    if (type_a->type == ZENIT_TYPE_STRUCT)
        return zenit_type_struct_equals((struct ZenitStructTypeInfo*) type_a, type_b);
    
    if (type_a->type == ZENIT_TYPE_REFERENCE)
        return zenit_type_reference_equals((struct ZenitReferenceTypeInfo*) type_a, type_b);
    
    if (type_a->type == ZENIT_TYPE_ARRAY)
        return zenit_type_array_equals((struct ZenitArrayTypeInfo*) type_a, type_b);
    
    if (zenit_type_is_primitive(type_a->type))
        return zenit_type_primitive_equals((struct ZenitPrimitiveTypeInfo*) type_a, type_b);
    
    if (type_a->type == ZENIT_TYPE_NONE)
        return type_b->type == ZENIT_TYPE_NONE;
    
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

bool zenit_type_is_assignable_from(struct ZenitTypeInfo *target_type, struct ZenitTypeInfo *from_type)
{
    if (target_type == NULL || from_type == NULL)
        return false;

    if (target_type->type == ZENIT_TYPE_NONE)
        return from_type->type != ZENIT_TYPE_NONE;

    if (target_type->type == ZENIT_TYPE_STRUCT)
        return zenit_type_struct_is_assignable_from((struct ZenitStructTypeInfo*) target_type, from_type);
    
    if (target_type->type == ZENIT_TYPE_REFERENCE)
        return zenit_type_reference_is_assignable_from((struct ZenitReferenceTypeInfo*) target_type, from_type);
    
    if (target_type->type == ZENIT_TYPE_ARRAY)
        return zenit_type_array_is_assignable_from((struct ZenitArrayTypeInfo*) target_type, from_type);
    
    if (zenit_type_is_primitive(target_type->type))
        return zenit_type_primitive_is_assignable_from((struct ZenitPrimitiveTypeInfo*) target_type, from_type);
    
    return false;
}


bool zenit_type_is_castable_to(struct ZenitTypeInfo *source_type, struct ZenitTypeInfo *target_type)
{
    // If we can assign from the target type to the cast type, it means
    // the types are compatible, and we could proceed with the down casting
    // by truncating the target_type to a source_type
    if (zenit_type_is_assignable_from(source_type, target_type))
        return true;

    // We can't cast things we don't know
    if (target_type->type == ZENIT_TYPE_NONE || source_type->type == ZENIT_TYPE_NONE)
        return false;

    bool target_is_uint = target_type->type >= ZENIT_TYPE_UINT8 && target_type->type <= ZENIT_TYPE_UINT16;
    bool source_is_uint = source_type->type >= ZENIT_TYPE_UINT8 && source_type->type <= ZENIT_TYPE_UINT16;

    // We can cast a reference to an unsigned integer
    if (source_type->type == ZENIT_TYPE_REFERENCE && target_is_uint)
        return true;

    if (target_type->type == ZENIT_TYPE_REFERENCE && target_type->type != source_type->type)
        return false;

    if (target_is_uint && source_is_uint)
        return true;

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
