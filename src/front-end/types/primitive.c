#include <fllib.h>
#include <stdlib.h>
#include "primitive.h"

static struct TypeMapping {
    char *string;
    enum ZenitType type;
} type_mappings[] = {
    { "<none>",     ZENIT_TYPE_NONE     },
    { "uint8",      ZENIT_TYPE_UINT8    },
    { "uint16",     ZENIT_TYPE_UINT16   },
};

enum ZenitType zenit_type_primitive_from_slice(struct FlSlice *slice)
{
    for (size_t i=0; i < sizeof(type_mappings) / sizeof(type_mappings[0]); i++)
    {
        struct TypeMapping mapping = type_mappings[i];
        if (fl_slice_equals_sequence(slice, (FlByte*)mapping.string, strlen(mapping.string)))
            return mapping.type;
    }

    return ZENIT_TYPE_NONE;
}

struct ZenitPrimitiveTypeInfo* zenit_type_primitive_new(enum ZenitType type)
{
    if (!zenit_type_is_primitive(type))
        return NULL;
        
    struct ZenitPrimitiveTypeInfo *typeinfo = fl_malloc(sizeof(struct ZenitPrimitiveTypeInfo));
    typeinfo->base.type = type;

    return typeinfo;
}

unsigned long zenit_type_primitive_hash(struct ZenitPrimitiveTypeInfo *typeinfo)
{
    static const char *format = "[primitive][t:%s]";

    char type_key[256] = { 0 };
    snprintf(type_key, 256, format, zenit_type_primitive_to_string(typeinfo));

    unsigned long hash = 5381;
    FlByte c;

    for (size_t i=0; i < strlen(type_key); i++)
        hash = ((hash << 5) + hash) + type_key[i];

    return hash;
}

struct ZenitPrimitiveTypeInfo* zenit_type_primitive_copy(struct ZenitPrimitiveTypeInfo *src_type)
{
    if (!src_type)
        return NULL;

    return zenit_type_primitive_new(src_type->base.type);
}

char* zenit_type_primitive_to_string(struct ZenitPrimitiveTypeInfo *typeinfo)
{
    for (size_t i=0; i < sizeof(type_mappings) / sizeof(type_mappings[0]); i++)
    {
        struct TypeMapping mapping = type_mappings[i];
        if (typeinfo->base.type == mapping.type)
            return mapping.string;
    }

    return NULL;
}

bool zenit_type_primitive_equals(struct ZenitPrimitiveTypeInfo *type_a, struct ZenitTypeInfo *type_b)
{
    if (type_a == NULL || type_b == NULL)
        return (struct ZenitTypeInfo*) type_a == type_b;

    if (!zenit_type_is_primitive(type_b->type))
        return false;

    return type_a->base.type == type_b->type;
}

bool zenit_type_primitive_is_assignable_from(struct ZenitPrimitiveTypeInfo *target_type, struct ZenitTypeInfo *from_type)
{
    if (target_type == NULL || from_type == NULL)
        return false;

    if (!zenit_type_is_primitive(from_type->type))
        return false;

    bool a_uint = target_type->base.type >= ZENIT_TYPE_UINT8 && target_type->base.type <= ZENIT_TYPE_UINT16;
    bool b_uint = from_type->type >= ZENIT_TYPE_UINT8 && from_type->type <= ZENIT_TYPE_UINT16;
    
    if (a_uint && b_uint)
    {
        if (target_type->base.type >= from_type->type)
            return true;
    }

    return false;
}

bool zenit_type_primitive_is_castable_to(struct ZenitPrimitiveTypeInfo *primitive_type, struct ZenitTypeInfo *target_type)
{
    if (target_type == NULL || target_type == NULL)
        return false;

    // If the target_type is of the same type, it is safe to cast
    if (zenit_type_primitive_equals(primitive_type, target_type))
        return true;

    bool primitive_is_uint = primitive_type->base.type >= ZENIT_TYPE_UINT8 && primitive_type->base.type <= ZENIT_TYPE_UINT16;
    bool target_is_uint = target_type->type >= ZENIT_TYPE_UINT8 && target_type->type <= ZENIT_TYPE_UINT16;    

    // It is safe to cast between uints
    return primitive_is_uint && target_is_uint;
}

bool zenit_type_primitive_unify(struct ZenitPrimitiveTypeInfo *primitive_type, struct ZenitTypeInfo *type_b, struct ZenitTypeInfo **unified)
{
    if (primitive_type == NULL || type_b == NULL)
        return false;

    if (type_b->type == ZENIT_TYPE_NONE)
    {
        if (unified)
            *unified = (struct ZenitTypeInfo*) zenit_type_primitive_copy(primitive_type);
        return true;
    }

    if (zenit_type_primitive_equals(primitive_type, type_b))
    {
        if (unified)
            *unified = (struct ZenitTypeInfo*) zenit_type_primitive_copy(primitive_type);
        return true;
    }

    if (!zenit_type_is_primitive(type_b->type))
        return false;

    bool primitive_is_uint = primitive_type->base.type >= ZENIT_TYPE_UINT8 && primitive_type->base.type <= ZENIT_TYPE_UINT16;
    bool b_uint = type_b->type >= ZENIT_TYPE_UINT8 && type_b->type <= ZENIT_TYPE_UINT16;
    
    if (primitive_is_uint != b_uint)
        return false;

    if (unified)
        *unified = zenit_type_copy(primitive_type->base.type > type_b->type ?  (struct ZenitTypeInfo*) primitive_type : type_b);
    
    return true;
}

void zenit_type_primitive_free(struct ZenitPrimitiveTypeInfo *typeinfo)
{
    if (!typeinfo)
        return;

    if (typeinfo->base.to_string.value != NULL)
        fl_cstring_free(typeinfo->base.to_string.value);

    fl_free(typeinfo);
}
