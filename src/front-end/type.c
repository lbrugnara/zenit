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
