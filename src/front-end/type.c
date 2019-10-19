#include <fllib.h>
#include "type.h"

struct TypeMapping {
    const char *string;
    CenitType type;
} type_mappings[] = {
    { "uint8", CENIT_TYPE_UINT8 }
};

CenitType cenit_type_parse(const char *typestr)
{
    for (size_t i=0; i < sizeof(type_mappings) / sizeof(type_mappings[0]); i++)
    {
        struct TypeMapping mapping = type_mappings[i];
        if (flm_cstring_equals(mapping.string, typestr))
            return mapping.type;
    }

    // We simply assume the type is a custom type
    return CENIT_TYPE_CUSTOM;
}

CenitType cenit_type_parse_slice(struct FlSlice *slice)
{
    for (size_t i=0; i < sizeof(type_mappings) / sizeof(type_mappings[0]); i++)
    {
        struct TypeMapping mapping = type_mappings[i];
        if (fl_slice_equals_sequence(slice, (FlByte*)mapping.string, strlen(mapping.string)))
            return mapping.type;
    }

    // We simply assume the type is a custom type
    return CENIT_TYPE_CUSTOM;
}

const char* cenit_type_string(CenitType type)
{
    for (size_t i=0; i < sizeof(type_mappings) / sizeof(type_mappings[0]); i++)
    {
        struct TypeMapping mapping = type_mappings[i];
        if (mapping.type == type)
            return mapping.string;
    }

    return "<none>";
}

void cenit_type_copy(CenitTypeInfo *dest_type, CenitTypeInfo *src_type)
{
    dest_type->name = src_type->name ? fl_cstring_dup(src_type->name) : NULL;
    dest_type->elements = src_type->elements;
    dest_type->is_array = src_type->is_array;
    dest_type->type = src_type->type;
}

bool cenit_type_equals(CenitTypeInfo *type_a, CenitTypeInfo *type_b)
{
    if (type_a->type != type_b->type)
        return false;

    if (type_a->name != type_b->name)
        return false;

    if (type_a->is_array != type_b->is_array)
        return false;

    return type_a->elements == type_b->elements;
}
