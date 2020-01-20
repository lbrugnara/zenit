#include <fllib.h>
#include <stdlib.h>
#include <limits.h>
#include "type.h"
#include "system.h"

unsigned long zir_type_hash(struct ZirType *type)
{
    if (!type)
        return ULONG_MAX;

    if (type->typekind == ZIR_TYPE_STRUCT)
        return zir_type_struct_hash((struct ZirStructType*) type);
    
    if (type->typekind == ZIR_TYPE_REFERENCE)
        return zir_type_reference_hash((struct ZirReferenceType*) type);
    
    if (type->typekind == ZIR_TYPE_ARRAY)
        return zir_type_array_hash((struct ZirArrayType*) type);
    
    if (type->typekind == ZIR_TYPE_UINT)
        return zir_type_uint_hash((struct ZirUintType*) type);
    
    if (type->typekind == ZIR_TYPE_NONE)
        return zir_type_none_hash(type);

    return 0;
}

char* zir_type_to_string(struct ZirType *type)
{
    if (!type)
        return NULL;

    unsigned long type_hash = zir_type_hash((struct ZirType*) type);

    if (type_hash == type->to_string.version && type->to_string.value != NULL)
    {
        // If the type information didn't change, just return the 
        // current value
        return type->to_string.value;
    }

    if (type->typekind == ZIR_TYPE_STRUCT)
        return zir_type_struct_to_string((struct ZirStructType*) type);
    
    if (type->typekind == ZIR_TYPE_REFERENCE)
        return zir_type_reference_to_string((struct ZirReferenceType*) type);
    
    if (type->typekind == ZIR_TYPE_ARRAY)
        return zir_type_array_to_string((struct ZirArrayType*) type);
    
    if (type->typekind == ZIR_TYPE_UINT)
        return zir_type_uint_to_string((struct ZirUintType*) type);
    
    if (type->typekind == ZIR_TYPE_NONE)
        return "<unknown>";

    return NULL;
}

struct ZirType* zir_type_copy(struct ZirType *src_type)
{
    if (!src_type)
        return NULL;

    if (src_type->typekind == ZIR_TYPE_STRUCT)
        return (struct ZirType*) zir_type_struct_copy((struct ZirStructType*) src_type);
    
    if (src_type->typekind == ZIR_TYPE_REFERENCE)
        return (struct ZirType*) zir_type_reference_copy((struct ZirReferenceType*) src_type);
    
    if (src_type->typekind == ZIR_TYPE_ARRAY)
        return (struct ZirType*) zir_type_array_copy((struct ZirArrayType*) src_type);
    
    if (src_type->typekind == ZIR_TYPE_UINT)
        return (struct ZirType*) zir_type_uint_copy((struct ZirUintType*) src_type);
    
    if (src_type->typekind == ZIR_TYPE_NONE)
        return zir_type_none_new();

    return NULL;
}

bool zir_type_equals(struct ZirType *type_a, struct ZirType *type_b)
{
    if (type_a == NULL || type_b == NULL)
        return type_a == type_b;

    if (type_a->typekind == ZIR_TYPE_STRUCT)
        return zir_type_struct_equals((struct ZirStructType*) type_a, type_b);
    
    if (type_a->typekind == ZIR_TYPE_REFERENCE)
        return zir_type_reference_equals((struct ZirReferenceType*) type_a, type_b);
    
    if (type_a->typekind == ZIR_TYPE_ARRAY)
        return zir_type_array_equals((struct ZirArrayType*) type_a, type_b);
    
    if (type_a->typekind == ZIR_TYPE_UINT)
        return zir_type_uint_equals((struct ZirUintType*) type_a, type_b);
    
    if (type_a->typekind == ZIR_TYPE_NONE)
        return type_b->typekind == ZIR_TYPE_NONE;
    
    return false;
}

bool zir_type_is_assignable_from(struct ZirType *target_type, struct ZirType *from_type)
{
    if (target_type == NULL || from_type == NULL)
        return false;

    if (target_type->typekind == ZIR_TYPE_NONE)
        return from_type->typekind != ZIR_TYPE_NONE;

    if (target_type->typekind == ZIR_TYPE_STRUCT)
        return zir_type_struct_is_assignable_from((struct ZirStructType*) target_type, from_type);
    
    if (target_type->typekind == ZIR_TYPE_REFERENCE)
        return zir_type_reference_is_assignable_from((struct ZirReferenceType*) target_type, from_type);
    
    if (target_type->typekind == ZIR_TYPE_ARRAY)
        return zir_type_array_is_assignable_from((struct ZirArrayType*) target_type, from_type);
    
    if (target_type->typekind == ZIR_TYPE_UINT)
        return zir_type_uint_is_assignable_from((struct ZirUintType*) target_type, from_type);
    
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

    if (source_type->typekind == ZIR_TYPE_STRUCT)
        return zir_type_struct_is_castable_to((struct ZirStructType*) source_type, target_type);
    
    if (source_type->typekind == ZIR_TYPE_REFERENCE)
        return zir_type_reference_is_castable_to((struct ZirReferenceType*) source_type, target_type);
    
    if (source_type->typekind == ZIR_TYPE_ARRAY)
        return zir_type_array_is_castable_to((struct ZirArrayType*) source_type, target_type);
    
    if (source_type->typekind == ZIR_TYPE_UINT)
        return zir_type_uint_is_castable_to((struct ZirUintType*) source_type, target_type);
    
    return false;
}

size_t zir_type_size(struct ZirType *type)
{
    if (!type)
        return 0;

    if (type->typekind == ZIR_TYPE_STRUCT)
        return zir_type_struct_size((struct ZirStructType*) type);
    
    if (type->typekind == ZIR_TYPE_REFERENCE)
        return zir_type_reference_size((struct ZirReferenceType*) type);
    
    if (type->typekind == ZIR_TYPE_ARRAY)
        return zir_type_array_size((struct ZirArrayType*) type);
    
    if (type->typekind == ZIR_TYPE_UINT)
        return zir_type_uint_size((struct ZirUintType*) type);

    return 0;
}

void zir_type_free(struct ZirType *type)
{
    if (!type)
        return;

    if (type->typekind == ZIR_TYPE_STRUCT)
    {
        zir_type_struct_free((struct ZirStructType*) type);
        return;
    }
    else if (type->typekind == ZIR_TYPE_REFERENCE)
    {
        zir_type_reference_free((struct ZirReferenceType*) type);
        return;
    }
    else if (type->typekind == ZIR_TYPE_ARRAY)
    {
        zir_type_array_free((struct ZirArrayType*) type);
        return;
    }
    else if (type->typekind == ZIR_TYPE_UINT)
    {
        zir_type_uint_free((struct ZirUintType*) type);
        return;
    }
    else if (type->typekind == ZIR_TYPE_NONE)
    {
        zir_type_none_free(type);
        return;
    }
}
