#include <fllib.h>
#include <stdlib.h>
#include "type.h"
#include "system.h"

enum ZenitType zenit_type_from_slice(struct FlSlice *slice)
{
    if (flm_cstring_equals_n("uint", (const char*) slice->sequence, 4))
        return ZENIT_TYPE_UINT;

    return ZENIT_TYPE_STRUCT;
}

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
    
    if (typeinfo->type == ZENIT_TYPE_UINT)
        return zenit_type_uint_hash((struct ZenitUintTypeInfo*) typeinfo);
    
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
    
    if (typeinfo->type == ZENIT_TYPE_UINT)
        return zenit_type_uint_to_string((struct ZenitUintTypeInfo*) typeinfo);
    
    if (typeinfo->type == ZENIT_TYPE_NONE)
        return "<unknown>";

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
    
    if (src_type->type == ZENIT_TYPE_UINT)
        return (struct ZenitTypeInfo*) zenit_type_uint_copy((struct ZenitUintTypeInfo*) src_type);
    
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
    
    if (type_a->type == ZENIT_TYPE_UINT)
        return zenit_type_uint_equals((struct ZenitUintTypeInfo*) type_a, type_b);
    
    if (type_a->type == ZENIT_TYPE_NONE)
        return type_b->type == ZENIT_TYPE_NONE;
    
    return false;
}

struct ZenitTypeInfo* zenit_type_unify(struct ZenitTypeInfo *type_a, struct ZenitTypeInfo *type_b)
{
    if (type_a == NULL || type_b == NULL)
        return NULL;

    if (type_a->type == ZENIT_TYPE_NONE)
    {
        if (type_b->type == ZENIT_TYPE_NONE)
            return NULL;

        struct ZenitTypeInfo *unified = zenit_type_copy(type_b);
        unified->source = ZENIT_TYPE_SRC_INFERRED;
        return unified;
    }

    if (type_a->type == ZENIT_TYPE_STRUCT)
        return zenit_type_struct_unify((struct ZenitStructTypeInfo*) type_a, type_b);
    
    if (type_a->type == ZENIT_TYPE_REFERENCE)
        return zenit_type_reference_unify((struct ZenitReferenceTypeInfo*) type_a, type_b);
    
    if (type_a->type == ZENIT_TYPE_ARRAY)
        return zenit_type_array_unify((struct ZenitArrayTypeInfo*) type_a, type_b);
    
    if (type_a->type == ZENIT_TYPE_UINT)
        return zenit_type_uint_unify((struct ZenitUintTypeInfo*) type_a, type_b);
    
    return NULL;
}

bool zenit_type_can_unify(struct ZenitTypeInfo *type_a, struct ZenitTypeInfo *type_b)
{
    if (type_a == NULL || type_b == NULL)
        return false;

    if (type_a->type == ZENIT_TYPE_NONE)
        return type_b->type != ZENIT_TYPE_NONE;

    if (type_a->type == ZENIT_TYPE_STRUCT)
        return zenit_type_struct_can_unify((struct ZenitStructTypeInfo*) type_a, type_b);
    
    if (type_a->type == ZENIT_TYPE_REFERENCE)
        return zenit_type_reference_can_unify((struct ZenitReferenceTypeInfo*) type_a, type_b);
    
    if (type_a->type == ZENIT_TYPE_ARRAY)
        return zenit_type_array_can_unify((struct ZenitArrayTypeInfo*) type_a, type_b);
    
    if (type_a->type == ZENIT_TYPE_UINT)
        return zenit_type_uint_can_unify((struct ZenitUintTypeInfo*) type_a, type_b);
    
    return false;
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
    
    if (target_type->type == ZENIT_TYPE_UINT)
        return zenit_type_uint_is_assignable_from((struct ZenitUintTypeInfo*) target_type, from_type);
    
    return false;
}

bool zenit_type_is_castable_to(struct ZenitTypeInfo *source_type, struct ZenitTypeInfo *target_type)
{
    // We can cast from or to NULL objects...
    if (source_type == NULL || target_type == NULL)
        return false;

    // We can't cast things we don't know
    if (target_type->type == ZENIT_TYPE_NONE || source_type->type == ZENIT_TYPE_NONE)
        return false;

    if (source_type->type == ZENIT_TYPE_STRUCT)
        return zenit_type_struct_is_castable_to((struct ZenitStructTypeInfo*) source_type, target_type);
    
    if (source_type->type == ZENIT_TYPE_REFERENCE)
        return zenit_type_reference_is_castable_to((struct ZenitReferenceTypeInfo*) source_type, target_type);
    
    if (source_type->type == ZENIT_TYPE_ARRAY)
        return zenit_type_array_is_castable_to((struct ZenitArrayTypeInfo*) source_type, target_type);
    
    if (source_type->type == ZENIT_TYPE_UINT)
        return zenit_type_uint_is_castable_to((struct ZenitUintTypeInfo*) source_type, target_type);
    
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
    else if (typeinfo->type == ZENIT_TYPE_UINT)
    {
        zenit_type_uint_free((struct ZenitUintTypeInfo*) typeinfo);
        return;
    }
    else if (typeinfo->type == ZENIT_TYPE_NONE)
    {
        zenit_type_none_free(typeinfo);
        return;
    }
}
