
#include <stdlib.h>
#include <limits.h>
#include <fllib/Cstring.h>
#include "type.h"
#include "context.h"

ZenitTypeKind zenit_type_from_slice(struct FlSlice *slice)
{
    if (flm_cstring_equals_n("uint", (const char*) slice->sequence, 4))
        return ZENIT_TYPE_UINT;

    if (flm_cstring_equals_n("bool", (const char*) slice->sequence, 4))
        return ZENIT_TYPE_BOOL;

    return ZENIT_TYPE_STRUCT;
}

unsigned long zenit_type_hash(ZenitType *type)
{
    if (!type)
        return ULONG_MAX;

    switch (type->typekind)
    {
        case ZENIT_TYPE_STRUCT:
            return zenit_type_struct_hash((ZenitStructType*) type);
        
        case ZENIT_TYPE_REFERENCE:
            return zenit_type_reference_hash((ZenitReferenceType*) type);
        
        case ZENIT_TYPE_ARRAY:
            return zenit_type_array_hash((ZenitArrayType*) type);
        
        case ZENIT_TYPE_UINT:
            return zenit_type_uint_hash((ZenitUintType*) type);

        case ZENIT_TYPE_BOOL:
            return zenit_type_bool_hash((ZenitBoolType*) type);
        
        case ZENIT_TYPE_NONE:
            return zenit_type_none_hash(type);
    }

    return 0;
}

char* zenit_type_to_string(ZenitType *type)
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

    switch (type->typekind)
    {
        case ZENIT_TYPE_STRUCT:
            return zenit_type_struct_to_string((ZenitStructType*) type);
        
        case ZENIT_TYPE_REFERENCE:
            return zenit_type_reference_to_string((ZenitReferenceType*) type);
        
        case ZENIT_TYPE_ARRAY:
            return zenit_type_array_to_string((ZenitArrayType*) type);
        
        case ZENIT_TYPE_UINT:
            return zenit_type_uint_to_string((ZenitUintType*) type);

        case ZENIT_TYPE_BOOL:
            return zenit_type_bool_to_string((ZenitBoolType*) type);
        
        case ZENIT_TYPE_NONE:
            return "<unknown>";
    }

    return NULL;
}

bool zenit_type_equals(ZenitType *type_a, ZenitType *type_b)
{
    if (type_a == NULL || type_b == NULL)
        return type_a == type_b;

    switch (type_a->typekind)
    {
        case ZENIT_TYPE_STRUCT:
            return zenit_type_struct_equals((ZenitStructType*) type_a, type_b);
        
        case ZENIT_TYPE_REFERENCE:
            return zenit_type_reference_equals((ZenitReferenceType*) type_a, type_b);
        
        case ZENIT_TYPE_ARRAY:
            return zenit_type_array_equals((ZenitArrayType*) type_a, type_b);
        
        case ZENIT_TYPE_UINT:
            return zenit_type_uint_equals((ZenitUintType*) type_a, type_b);

        case ZENIT_TYPE_BOOL:
            return zenit_type_bool_equals((ZenitBoolType*) type_a, type_b);
        
        case ZENIT_TYPE_NONE:
            return type_b->typekind == ZENIT_TYPE_NONE;
    }
    
    return false;
}

bool zenit_type_can_unify(ZenitType *type_a, ZenitType *type_b)
{
    if (type_a == NULL || type_b == NULL)
        return false;

    switch (type_a->typekind)
    {
        case ZENIT_TYPE_NONE:
            return type_b->typekind != ZENIT_TYPE_NONE;

        case ZENIT_TYPE_STRUCT:
            return zenit_type_struct_can_unify((ZenitStructType*) type_a, type_b);
        
        case ZENIT_TYPE_REFERENCE:
            return zenit_type_reference_can_unify((ZenitReferenceType*) type_a, type_b);
        
        case ZENIT_TYPE_ARRAY:
            return zenit_type_array_can_unify((ZenitArrayType*) type_a, type_b);
        
        case ZENIT_TYPE_UINT:
            return zenit_type_uint_can_unify((ZenitUintType*) type_a, type_b);

        case ZENIT_TYPE_BOOL:
            return zenit_type_bool_can_unify((ZenitBoolType*) type_a, type_b);
    }
    
    return false;
}

bool zenit_type_is_assignable_from(ZenitType *target_type, ZenitType *from_type)
{
    if (target_type == NULL || from_type == NULL)
        return false;

    switch (target_type->typekind)
    {
        case ZENIT_TYPE_NONE:
            return from_type->typekind != ZENIT_TYPE_NONE;

        case ZENIT_TYPE_STRUCT:
            return zenit_type_struct_is_assignable_from((ZenitStructType*) target_type, from_type);
        
        case ZENIT_TYPE_REFERENCE:
            return zenit_type_reference_is_assignable_from((ZenitReferenceType*) target_type, from_type);
        
        case ZENIT_TYPE_ARRAY:
            return zenit_type_array_is_assignable_from((ZenitArrayType*) target_type, from_type);
        
        case ZENIT_TYPE_UINT:
            return zenit_type_uint_is_assignable_from((ZenitUintType*) target_type, from_type);

        case ZENIT_TYPE_BOOL:
            return zenit_type_bool_is_assignable_from((ZenitBoolType*) target_type, from_type);
    }
    
    return false;
}

bool zenit_type_is_castable_to(ZenitType *source_type, ZenitType *target_type)
{
    // We can cast from or to NULL objects...
    if (source_type == NULL || target_type == NULL)
        return false;

    // We can't cast things we don't know
    if (target_type->typekind == ZENIT_TYPE_NONE || source_type->typekind == ZENIT_TYPE_NONE)
        return false;

    switch (source_type->typekind)
    {
        case ZENIT_TYPE_STRUCT:
            return zenit_type_struct_is_castable_to((ZenitStructType*) source_type, target_type);
        
        case ZENIT_TYPE_REFERENCE:
            return zenit_type_reference_is_castable_to((ZenitReferenceType*) source_type, target_type);
        
        case ZENIT_TYPE_ARRAY:
            return zenit_type_array_is_castable_to((ZenitArrayType*) source_type, target_type);
        
        case ZENIT_TYPE_UINT:
            return zenit_type_uint_is_castable_to((ZenitUintType*) source_type, target_type);

        case ZENIT_TYPE_BOOL:
            return zenit_type_bool_is_castable_to((ZenitBoolType*) source_type, target_type);

        case ZENIT_TYPE_NONE: // Mute warning
            break;
    }
    
    return false;
}

void zenit_type_free(ZenitType *type)
{
    if (!type)
        return;

    switch (type->typekind)
    {
        case ZENIT_TYPE_STRUCT:
            zenit_type_struct_free((ZenitStructType*) type);
            break;

        case ZENIT_TYPE_REFERENCE:
            zenit_type_reference_free((ZenitReferenceType*) type);
            break;

        case ZENIT_TYPE_ARRAY:
            zenit_type_array_free((ZenitArrayType*) type);
            break;

        case ZENIT_TYPE_UINT:
            zenit_type_uint_free((ZenitUintType*) type);
            break;

        case ZENIT_TYPE_BOOL:
            zenit_type_bool_free((ZenitBoolType*) type);
            break;

        case ZENIT_TYPE_NONE:
            zenit_type_none_free(type);
            break;
    }
}
