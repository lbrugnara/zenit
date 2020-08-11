
#include <stdbool.h>
#include "type.h"
#include <stdlib.h>
#include <limits.h>
#include "system.h"

unsigned long zir_type_hash(ZirType *type)
{
    if (!type)
        return ULONG_MAX;

    switch (type->typekind)
    {
        case ZIR_TYPE_UINT:
            return zir_uint_type_hash((ZirUintType*) type);

        case ZIR_TYPE_BOOL:
            return zir_bool_type_hash((ZirBoolType*) type);

        case ZIR_TYPE_STRUCT:
            return zir_struct_type_hash((ZirStructType*) type);
        
        case ZIR_TYPE_REFERENCE:
            return zir_reference_type_hash((ZirReferenceType*) type);
        
        case ZIR_TYPE_ARRAY:
            return zir_array_type_hash((ZirArrayType*) type);
        
        case ZIR_TYPE_NONE:
            return zir_none_type_hash(type);
    }

    return 0;
}

char* zir_type_to_string(ZirType *type)
{
    if (!type)
        return NULL;

    unsigned long type_hash = zir_type_hash(type);

    if (type_hash == type->to_string.version && type->to_string.value != NULL)
    {
        // If the type information didn't change, just return the 
        // current value
        return type->to_string.value;
    }


    switch (type->typekind)
    {
        case ZIR_TYPE_UINT:
            return zir_uint_type_to_string((ZirUintType*) type);
            
        case ZIR_TYPE_BOOL:
            return zir_bool_type_to_string((ZirBoolType*) type);

        case ZIR_TYPE_STRUCT:
            return zir_struct_type_to_string((ZirStructType*) type);
        
        case ZIR_TYPE_REFERENCE:
            return zir_reference_type_to_string((ZirReferenceType*) type);
        
        case ZIR_TYPE_ARRAY:
            return zir_array_type_to_string((ZirArrayType*) type);
        
        case ZIR_TYPE_NONE:
            return "<unknown>";
    }

    return NULL;
}

bool zir_type_equals(ZirType *type_a, ZirType *type_b)
{
    if (type_a == NULL || type_b == NULL)
        return type_a == type_b;

    switch (type_a->typekind)
    {
        case ZIR_TYPE_UINT:
            return zir_uint_type_equals((ZirUintType*) type_a, type_b);

        case ZIR_TYPE_BOOL:
            return zir_bool_type_equals((ZirBoolType*) type_a, type_b);

        case ZIR_TYPE_STRUCT:
            return zir_struct_type_equals((ZirStructType*) type_a, type_b);
        
        case ZIR_TYPE_REFERENCE:
            return zir_reference_type_equals((ZirReferenceType*) type_a, type_b);
        
        case ZIR_TYPE_ARRAY:
            return zir_array_type_equals((ZirArrayType*) type_a, type_b);
        
        case ZIR_TYPE_NONE:
            return type_b->typekind == ZIR_TYPE_NONE;
    }
    
    return false;
}

bool zir_type_is_assignable_from(ZirType *target_type, ZirType *from_type)
{
    if (target_type == NULL || from_type == NULL)
        return false;

    switch (target_type->typekind)
    {
        case ZIR_TYPE_NONE:
            return from_type->typekind != ZIR_TYPE_NONE;
        
        case ZIR_TYPE_UINT:
            return zir_uint_type_is_assignable_from((ZirUintType*) target_type, from_type);

        case ZIR_TYPE_BOOL:
            return zir_bool_type_is_assignable_from((ZirBoolType*) target_type, from_type);

        case ZIR_TYPE_STRUCT:
            return zir_struct_type_is_assignable_from((ZirStructType*) target_type, from_type);
        
        case ZIR_TYPE_REFERENCE:
            return zir_reference_type_is_assignable_from((ZirReferenceType*) target_type, from_type);
        
        case ZIR_TYPE_ARRAY:
            return zir_array_type_is_assignable_from((ZirArrayType*) target_type, from_type);
    }
    
    return false;
}

bool zir_type_is_castable_to(ZirType *source_type, ZirType *target_type)
{
    // We can cast from or to NULL objects...
    if (source_type == NULL || target_type == NULL)
        return false;

    // We can't cast things we don't know
    if (target_type->typekind == ZIR_TYPE_NONE || source_type->typekind == ZIR_TYPE_NONE)
        return false;

    switch (source_type->typekind)
    {        
        case ZIR_TYPE_UINT:
            return zir_uint_type_is_castable_to((ZirUintType*) source_type, target_type);

        case ZIR_TYPE_BOOL:
            return zir_bool_type_is_castable_to((ZirBoolType*) source_type, target_type);

        case ZIR_TYPE_STRUCT:
            return zir_struct_type_is_castable_to((ZirStructType*) source_type, target_type);
        
        case ZIR_TYPE_REFERENCE:
            return zir_reference_type_is_castable_to((ZirReferenceType*) source_type, target_type);
        
        case ZIR_TYPE_ARRAY:
            return zir_array_type_is_castable_to((ZirArrayType*) source_type, target_type);
        
        case ZIR_TYPE_NONE: // Mute warning
            break;
    }
    
    return false;
}

size_t zir_type_size(ZirType *type)
{
    if (!type)
        return 0;

    switch (type->typekind)
    {
        case ZIR_TYPE_UINT:
            return zir_uint_type_size((ZirUintType*) type);

        case ZIR_TYPE_BOOL:
            return zir_bool_type_size((ZirBoolType*) type);

        case ZIR_TYPE_STRUCT:
            return zir_struct_type_size((ZirStructType*) type);
        
        case ZIR_TYPE_REFERENCE:
            return zir_reference_type_size((ZirReferenceType*) type);
        
        case ZIR_TYPE_ARRAY:
            return zir_array_type_size((ZirArrayType*) type);
        
        case ZIR_TYPE_NONE:
            break;
    }

    return 0;
}

void zir_type_free(ZirType *type)
{
    if (!type)
        return;

    switch (type->typekind)
    {
        
        case ZIR_TYPE_UINT:
            zir_uint_type_free((ZirUintType*) type);
            break;

        case ZIR_TYPE_BOOL:
            zir_bool_type_free((ZirBoolType*) type);
            break;

        case ZIR_TYPE_STRUCT:
            zir_struct_type_free((ZirStructType*) type);
            break;

        case ZIR_TYPE_REFERENCE:
            zir_reference_type_free((ZirReferenceType*) type);
            break;

        case ZIR_TYPE_ARRAY:
            zir_array_type_free((ZirArrayType*) type);
            break;

        case ZIR_TYPE_NONE:
            zir_none_type_free(type);
            break;
    }
}
