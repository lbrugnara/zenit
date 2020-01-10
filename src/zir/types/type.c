#include <fllib.h>
#include <stdlib.h>
#include <limits.h>
#include "type.h"
#include "system.h"

unsigned long zir_type_hash(struct ZirTypeInfo *typeinfo)
{
    if (!typeinfo)
        return ULONG_MAX;

    if (typeinfo->type == ZIR_TYPE_STRUCT)
        return zir_type_struct_hash((struct ZirStructTypeInfo*) typeinfo);
    
    if (typeinfo->type == ZIR_TYPE_REFERENCE)
        return zir_type_reference_hash((struct ZirReferenceTypeInfo*) typeinfo);
    
    if (typeinfo->type == ZIR_TYPE_ARRAY)
        return zir_type_array_hash((struct ZirArrayTypeInfo*) typeinfo);
    
    if (typeinfo->type == ZIR_TYPE_UINT)
        return zir_type_uint_hash((struct ZirUintTypeInfo*) typeinfo);
    
    if (typeinfo->type == ZIR_TYPE_NONE)
        return zir_type_none_hash(typeinfo);

    return 0;
}

char* zir_type_to_string(struct ZirTypeInfo *typeinfo)
{
    if (!typeinfo)
        return NULL;

    unsigned long type_hash = zir_type_hash((struct ZirTypeInfo*) typeinfo);

    if (type_hash == typeinfo->to_string.version && typeinfo->to_string.value != NULL)
    {
        // If the type information didn't change, just return the 
        // current value
        return typeinfo->to_string.value;
    }

    if (typeinfo->type == ZIR_TYPE_STRUCT)
        return zir_type_struct_to_string((struct ZirStructTypeInfo*) typeinfo);
    
    if (typeinfo->type == ZIR_TYPE_REFERENCE)
        return zir_type_reference_to_string((struct ZirReferenceTypeInfo*) typeinfo);
    
    if (typeinfo->type == ZIR_TYPE_ARRAY)
        return zir_type_array_to_string((struct ZirArrayTypeInfo*) typeinfo);
    
    if (typeinfo->type == ZIR_TYPE_UINT)
        return zir_type_uint_to_string((struct ZirUintTypeInfo*) typeinfo);
    
    if (typeinfo->type == ZIR_TYPE_NONE)
        return "<unknown>";

    return NULL;
}

struct ZirTypeInfo* zir_type_copy(struct ZirTypeInfo *src_type)
{
    if (!src_type)
        return NULL;

    if (src_type->type == ZIR_TYPE_STRUCT)
        return (struct ZirTypeInfo*) zir_type_struct_copy((struct ZirStructTypeInfo*) src_type);
    
    if (src_type->type == ZIR_TYPE_REFERENCE)
        return (struct ZirTypeInfo*) zir_type_reference_copy((struct ZirReferenceTypeInfo*) src_type);
    
    if (src_type->type == ZIR_TYPE_ARRAY)
        return (struct ZirTypeInfo*) zir_type_array_copy((struct ZirArrayTypeInfo*) src_type);
    
    if (src_type->type == ZIR_TYPE_UINT)
        return (struct ZirTypeInfo*) zir_type_uint_copy((struct ZirUintTypeInfo*) src_type);
    
    if (src_type->type == ZIR_TYPE_NONE)
        return zir_type_none_new();

    return NULL;
}

bool zir_type_equals(struct ZirTypeInfo *type_a, struct ZirTypeInfo *type_b)
{
    if (type_a == NULL || type_b == NULL)
        return type_a == type_b;

    if (type_a->type == ZIR_TYPE_STRUCT)
        return zir_type_struct_equals((struct ZirStructTypeInfo*) type_a, type_b);
    
    if (type_a->type == ZIR_TYPE_REFERENCE)
        return zir_type_reference_equals((struct ZirReferenceTypeInfo*) type_a, type_b);
    
    if (type_a->type == ZIR_TYPE_ARRAY)
        return zir_type_array_equals((struct ZirArrayTypeInfo*) type_a, type_b);
    
    if (type_a->type == ZIR_TYPE_UINT)
        return zir_type_uint_equals((struct ZirUintTypeInfo*) type_a, type_b);
    
    if (type_a->type == ZIR_TYPE_NONE)
        return type_b->type == ZIR_TYPE_NONE;
    
    return false;
}

bool zir_type_is_assignable_from(struct ZirTypeInfo *target_type, struct ZirTypeInfo *from_type)
{
    if (target_type == NULL || from_type == NULL)
        return false;

    if (target_type->type == ZIR_TYPE_NONE)
        return from_type->type != ZIR_TYPE_NONE;

    if (target_type->type == ZIR_TYPE_STRUCT)
        return zir_type_struct_is_assignable_from((struct ZirStructTypeInfo*) target_type, from_type);
    
    if (target_type->type == ZIR_TYPE_REFERENCE)
        return zir_type_reference_is_assignable_from((struct ZirReferenceTypeInfo*) target_type, from_type);
    
    if (target_type->type == ZIR_TYPE_ARRAY)
        return zir_type_array_is_assignable_from((struct ZirArrayTypeInfo*) target_type, from_type);
    
    if (target_type->type == ZIR_TYPE_UINT)
        return zir_type_uint_is_assignable_from((struct ZirUintTypeInfo*) target_type, from_type);
    
    return false;
}

bool zir_type_is_castable_to(struct ZirTypeInfo *source_type, struct ZirTypeInfo *target_type)
{
    // We can cast from or to NULL objects...
    if (source_type == NULL || target_type == NULL)
        return false;

    // We can't cast things we don't know
    if (target_type->type == ZIR_TYPE_NONE || source_type->type == ZIR_TYPE_NONE)
        return false;

    if (source_type->type == ZIR_TYPE_STRUCT)
        return zir_type_struct_is_castable_to((struct ZirStructTypeInfo*) source_type, target_type);
    
    if (source_type->type == ZIR_TYPE_REFERENCE)
        return zir_type_reference_is_castable_to((struct ZirReferenceTypeInfo*) source_type, target_type);
    
    if (source_type->type == ZIR_TYPE_ARRAY)
        return zir_type_array_is_castable_to((struct ZirArrayTypeInfo*) source_type, target_type);
    
    if (source_type->type == ZIR_TYPE_UINT)
        return zir_type_uint_is_castable_to((struct ZirUintTypeInfo*) source_type, target_type);
    
    return false;
}

size_t zir_type_size(struct ZirTypeInfo *typeinfo)
{
    if (!typeinfo)
        return 0;

    if (typeinfo->type == ZIR_TYPE_STRUCT)
        return zir_type_struct_size((struct ZirStructTypeInfo*) typeinfo);
    
    if (typeinfo->type == ZIR_TYPE_REFERENCE)
        return zir_type_reference_size((struct ZirReferenceTypeInfo*) typeinfo);
    
    if (typeinfo->type == ZIR_TYPE_ARRAY)
        return zir_type_array_size((struct ZirArrayTypeInfo*) typeinfo);
    
    if (typeinfo->type == ZIR_TYPE_UINT)
        return zir_type_uint_size((struct ZirUintTypeInfo*) typeinfo);

    return 0;
}

void zir_type_free(struct ZirTypeInfo *typeinfo)
{
    if (!typeinfo)
        return;

    if (typeinfo->type == ZIR_TYPE_STRUCT)
    {
        zir_type_struct_free((struct ZirStructTypeInfo*) typeinfo);
        return;
    }
    else if (typeinfo->type == ZIR_TYPE_REFERENCE)
    {
        zir_type_reference_free((struct ZirReferenceTypeInfo*) typeinfo);
        return;
    }
    else if (typeinfo->type == ZIR_TYPE_ARRAY)
    {
        zir_type_array_free((struct ZirArrayTypeInfo*) typeinfo);
        return;
    }
    else if (typeinfo->type == ZIR_TYPE_UINT)
    {
        zir_type_uint_free((struct ZirUintTypeInfo*) typeinfo);
        return;
    }
    else if (typeinfo->type == ZIR_TYPE_NONE)
    {
        zir_type_none_free(typeinfo);
        return;
    }
}
