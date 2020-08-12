#include <stdlib.h>
#include <fllib/Cstring.h>
#include "bool.h"

ZirBoolType* zir_bool_type_new(void)
{
    ZirBoolType *type = fl_malloc(sizeof(ZirBoolType));
    type->base.typekind = ZIR_TYPE_BOOL;

    return type;
}

unsigned long zir_bool_type_hash(ZirBoolType *type)
{
    const char *type_key = "[bool]";
    unsigned long hash = 5381;
    FlByte c;

    for (size_t i=0; i < strlen(type_key); i++)
        hash = ((hash << 5) + hash) + type_key[i];

    return hash;
}

char* zir_bool_type_to_string(ZirBoolType *type)
{
    return "bool";
}

bool zir_bool_type_equals(ZirBoolType *type_a, ZirType *type_b)
{
    if (type_a == NULL || type_b == NULL)
        return (ZirType*) type_a == type_b;

    return type_b->typekind == ZIR_TYPE_BOOL;
}

bool zir_bool_type_is_assignable_from(ZirBoolType *target_type, ZirType *from_type)
{
    if (target_type == NULL || from_type == NULL)
        return false;

    return from_type->typekind == ZIR_TYPE_BOOL;
}

bool zir_bool_type_is_castable_to(ZirBoolType *bool_type, ZirType *target_type)
{
    if (target_type == NULL || target_type == NULL)
        return false;

    // It is safe to cast between booleans
    return target_type->typekind == ZIR_TYPE_BOOL;
}

size_t zir_bool_type_size(ZirBoolType *type, size_t ref_size)
{
    if (!type)
        return 0;

    return 1;
}

void zir_bool_type_free(ZirBoolType *type)
{
    if (!type)
        return;

    if (type->base.to_string.value != NULL)
        fl_cstring_free(type->base.to_string.value);

    fl_free(type);
}
