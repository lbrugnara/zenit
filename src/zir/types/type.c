
#include <stdbool.h>
#include "type.h"
#include <stdlib.h>
#include <limits.h>
#include "system.h"

unsigned long zir_type_hash(struct ZirType *type)
{
    if (!type)
        return ULONG_MAX;

    switch (type->typekind)
    {
        case ZIR_TYPE_STRUCT:
            return zir_type_struct_hash((struct ZirStructType*) type);
        
        case ZIR_TYPE_REFERENCE:
            return zir_type_reference_hash((struct ZirReferenceType*) type);
        
        case ZIR_TYPE_ARRAY:
            return zir_type_array_hash((struct ZirArrayType*) type);
        
        case ZIR_TYPE_UINT:
            return zir_type_uint_hash((struct ZirUintType*) type);
        
        case ZIR_TYPE_NONE:
            return zir_type_none_hash(type);
    }

    return 0;
}

char* zir_type_to_string(struct ZirType *type)
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
        case ZIR_TYPE_STRUCT:
            return zir_type_struct_to_string((struct ZirStructType*) type);
        
        case ZIR_TYPE_REFERENCE:
            return zir_type_reference_to_string((struct ZirReferenceType*) type);
        
        case ZIR_TYPE_ARRAY:
            return zir_type_array_to_string((struct ZirArrayType*) type);
        
        case ZIR_TYPE_UINT:
            return zir_type_uint_to_string((struct ZirUintType*) type);
        
        case ZIR_TYPE_NONE:
            return "<unknown>";
    }

    return NULL;
}

bool zir_type_equals(struct ZirType *type_a, struct ZirType *type_b)
{
    if (type_a == NULL || type_b == NULL)
        return type_a == type_b;

    switch (type_a->typekind)
    {
        case ZIR_TYPE_STRUCT:
            return zir_type_struct_equals((struct ZirStructType*) type_a, type_b);
        
        case ZIR_TYPE_REFERENCE:
            return zir_type_reference_equals((struct ZirReferenceType*) type_a, type_b);
        
        case ZIR_TYPE_ARRAY:
            return zir_type_array_equals((struct ZirArrayType*) type_a, type_b);
        
        case ZIR_TYPE_UINT:
            return zir_type_uint_equals((struct ZirUintType*) type_a, type_b);
        
        case ZIR_TYPE_NONE:
            return type_b->typekind == ZIR_TYPE_NONE;
    }
    
    return false;
}

bool zir_type_is_assignable_from(struct ZirType *target_type, struct ZirType *from_type)
{
    if (target_type == NULL || from_type == NULL)
        return false;

    switch (target_type->typekind)
    {
        case ZIR_TYPE_NONE:
            return from_type->typekind != ZIR_TYPE_NONE;

        case ZIR_TYPE_STRUCT:
            return zir_type_struct_is_assignable_from((struct ZirStructType*) target_type, from_type);
        
        case ZIR_TYPE_REFERENCE:
            return zir_type_reference_is_assignable_from((struct ZirReferenceType*) target_type, from_type);
        
        case ZIR_TYPE_ARRAY:
            return zir_type_array_is_assignable_from((struct ZirArrayType*) target_type, from_type);
        
        case ZIR_TYPE_UINT:
            return zir_type_uint_is_assignable_from((struct ZirUintType*) target_type, from_type);
    }
    
    return false;
}

bool zir_type_is_castable_to(struct ZirType *source_type, struct ZirType *target_type)
{
    // We can cast from or to NULL objects...
    if (source_type == NULL || target_type == NULL)
        return false;

    // We can't cast things we don't know
    if (target_type->typekind == ZIR_TYPE_NONE || source_type->typekind == ZIR_TYPE_NONE)
        return false;

    switch (source_type->typekind)
    {
        case ZIR_TYPE_STRUCT:
            return zir_type_struct_is_castable_to((struct ZirStructType*) source_type, target_type);
        
        case ZIR_TYPE_REFERENCE:
            return zir_type_reference_is_castable_to((struct ZirReferenceType*) source_type, target_type);
        
        case ZIR_TYPE_ARRAY:
            return zir_type_array_is_castable_to((struct ZirArrayType*) source_type, target_type);
        
        case ZIR_TYPE_UINT:
            return zir_type_uint_is_castable_to((struct ZirUintType*) source_type, target_type);
        
        case ZIR_TYPE_NONE: // Mute warning
            break;
    }
    
    return false;
}

size_t zir_type_size(struct ZirType *type)
{
    if (!type)
        return 0;

    switch (type->typekind)
    {
        case ZIR_TYPE_STRUCT:
            return zir_type_struct_size((struct ZirStructType*) type);
        
        case ZIR_TYPE_REFERENCE:
            return zir_type_reference_size((struct ZirReferenceType*) type);
        
        case ZIR_TYPE_ARRAY:
            return zir_type_array_size((struct ZirArrayType*) type);
        
        case ZIR_TYPE_UINT:
            return zir_type_uint_size((struct ZirUintType*) type);

        case ZIR_TYPE_NONE:
            break;
    }

    return 0;
}

void zir_type_free(struct ZirType *type)
{
    if (!type)
        return;

    switch (type->typekind)
    {
        case ZIR_TYPE_STRUCT:
            zir_type_struct_free((struct ZirStructType*) type);
            break;

        case ZIR_TYPE_REFERENCE:
            zir_type_reference_free((struct ZirReferenceType*) type);
            break;

        case ZIR_TYPE_ARRAY:
            zir_type_array_free((struct ZirArrayType*) type);
            break;

        case ZIR_TYPE_UINT:
            zir_type_uint_free((struct ZirUintType*) type);
            break;

        case ZIR_TYPE_NONE:
            zir_type_none_free(type);
            break;
    }
}
