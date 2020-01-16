#include <fllib.h>
#include <stdlib.h>
#include <limits.h>
#include "type.h"
#include "system.h"

enum ZenitTypeKind zenit_type_from_slice(struct FlSlice *slice)
{
    if (flm_cstring_equals_n("uint", (const char*) slice->sequence, 4))
        return ZENIT_TYPE_UINT;

    return ZENIT_TYPE_STRUCT;
}

unsigned long zenit_type_hash(struct ZenitType *type)
{
    if (!type)
        return ULONG_MAX;

    if (type->typekind == ZENIT_TYPE_STRUCT)
        return zenit_type_struct_hash((struct ZenitStructType*) type);
    
    if (type->typekind == ZENIT_TYPE_REFERENCE)
        return zenit_type_reference_hash((struct ZenitReferenceType*) type);
    
    if (type->typekind == ZENIT_TYPE_ARRAY)
        return zenit_type_array_hash((struct ZenitArrayType*) type);
    
    if (type->typekind == ZENIT_TYPE_UINT)
        return zenit_type_uint_hash((struct ZenitUintType*) type);
    
    if (type->typekind == ZENIT_TYPE_NONE)
        return zenit_type_none_hash(type);

    return 0;
}

char* zenit_type_to_string(struct ZenitType *type)
{
    if (!type)
        return NULL;

    unsigned long type_hash = zenit_type_hash(type);

    if (type_hash == type->to_string.version && type->to_string.value != NULL)
    {
        // If the type information didn't change, just return the 
        // current value
        return type->to_string.value;
    }

    if (type->typekind == ZENIT_TYPE_STRUCT)
        return zenit_type_struct_to_string((struct ZenitStructType*) type);
    
    if (type->typekind == ZENIT_TYPE_REFERENCE)
        return zenit_type_reference_to_string((struct ZenitReferenceType*) type);
    
    if (type->typekind == ZENIT_TYPE_ARRAY)
        return zenit_type_array_to_string((struct ZenitArrayType*) type);
    
    if (type->typekind == ZENIT_TYPE_UINT)
        return zenit_type_uint_to_string((struct ZenitUintType*) type);
    
    if (type->typekind == ZENIT_TYPE_NONE)
        return "<unknown>";

    return NULL;
}

bool zenit_type_equals(struct ZenitType *type_a, struct ZenitType *type_b)
{
    if (type_a == NULL || type_b == NULL)
        return type_a == type_b;

    if (type_a->typekind == ZENIT_TYPE_STRUCT)
        return zenit_type_struct_equals((struct ZenitStructType*) type_a, type_b);
    
    if (type_a->typekind == ZENIT_TYPE_REFERENCE)
        return zenit_type_reference_equals((struct ZenitReferenceType*) type_a, type_b);
    
    if (type_a->typekind == ZENIT_TYPE_ARRAY)
        return zenit_type_array_equals((struct ZenitArrayType*) type_a, type_b);
    
    if (type_a->typekind == ZENIT_TYPE_UINT)
        return zenit_type_uint_equals((struct ZenitUintType*) type_a, type_b);
    
    if (type_a->typekind == ZENIT_TYPE_NONE)
        return type_b->typekind == ZENIT_TYPE_NONE;
    
    return false;
}

bool zenit_type_can_unify(struct ZenitType *type_a, struct ZenitType *type_b)
{
    if (type_a == NULL || type_b == NULL)
        return false;

    if (type_a->typekind == ZENIT_TYPE_NONE)
        return type_b->typekind != ZENIT_TYPE_NONE;

    if (type_a->typekind == ZENIT_TYPE_STRUCT)
        return zenit_type_struct_can_unify((struct ZenitStructType*) type_a, type_b);
    
    if (type_a->typekind == ZENIT_TYPE_REFERENCE)
        return zenit_type_reference_can_unify((struct ZenitReferenceType*) type_a, type_b);
    
    if (type_a->typekind == ZENIT_TYPE_ARRAY)
        return zenit_type_array_can_unify((struct ZenitArrayType*) type_a, type_b);
    
    if (type_a->typekind == ZENIT_TYPE_UINT)
        return zenit_type_uint_can_unify((struct ZenitUintType*) type_a, type_b);
    
    return false;
}

bool zenit_type_is_assignable_from(struct ZenitType *target_type, struct ZenitType *from_type)
{
    if (target_type == NULL || from_type == NULL)
        return false;

    if (target_type->typekind == ZENIT_TYPE_NONE)
        return from_type->typekind != ZENIT_TYPE_NONE;

    if (target_type->typekind == ZENIT_TYPE_STRUCT)
        return zenit_type_struct_is_assignable_from((struct ZenitStructType*) target_type, from_type);
    
    if (target_type->typekind == ZENIT_TYPE_REFERENCE)
        return zenit_type_reference_is_assignable_from((struct ZenitReferenceType*) target_type, from_type);
    
    if (target_type->typekind == ZENIT_TYPE_ARRAY)
        return zenit_type_array_is_assignable_from((struct ZenitArrayType*) target_type, from_type);
    
    if (target_type->typekind == ZENIT_TYPE_UINT)
        return zenit_type_uint_is_assignable_from((struct ZenitUintType*) target_type, from_type);
    
    return false;
}

bool zenit_type_is_castable_to(struct ZenitType *source_type, struct ZenitType *target_type)
{
    // We can cast from or to NULL objects...
    if (source_type == NULL || target_type == NULL)
        return false;

    // We can't cast things we don't know
    if (target_type->typekind == ZENIT_TYPE_NONE || source_type->typekind == ZENIT_TYPE_NONE)
        return false;

    if (source_type->typekind == ZENIT_TYPE_STRUCT)
        return zenit_type_struct_is_castable_to((struct ZenitStructType*) source_type, target_type);
    
    if (source_type->typekind == ZENIT_TYPE_REFERENCE)
        return zenit_type_reference_is_castable_to((struct ZenitReferenceType*) source_type, target_type);
    
    if (source_type->typekind == ZENIT_TYPE_ARRAY)
        return zenit_type_array_is_castable_to((struct ZenitArrayType*) source_type, target_type);
    
    if (source_type->typekind == ZENIT_TYPE_UINT)
        return zenit_type_uint_is_castable_to((struct ZenitUintType*) source_type, target_type);
    
    return false;
}

void zenit_type_free(struct ZenitType *type)
{
    if (!type)
        return;

    if (type->typekind == ZENIT_TYPE_STRUCT)
    {
        zenit_type_struct_free((struct ZenitStructType*) type);
        return;
    }
    else if (type->typekind == ZENIT_TYPE_REFERENCE)
    {
        zenit_type_reference_free((struct ZenitReferenceType*) type);
        return;
    }
    else if (type->typekind == ZENIT_TYPE_ARRAY)
    {
        zenit_type_array_free((struct ZenitArrayType*) type);
        return;
    }
    else if (type->typekind == ZENIT_TYPE_UINT)
    {
        zenit_type_uint_free((struct ZenitUintType*) type);
        return;
    }
    else if (type->typekind == ZENIT_TYPE_NONE)
    {
        zenit_type_none_free(type);
        return;
    }
}
