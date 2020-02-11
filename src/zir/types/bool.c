#include <stdlib.h>
#include <fllib/Cstring.h>
#include "bool.h"

struct ZirBoolType* zir_type_bool_new(void)
{
    struct ZirBoolType *type = fl_malloc(sizeof(struct ZirBoolType));
    type->base.typekind = ZIR_TYPE_BOOL;

    return type;
}

unsigned long zir_type_bool_hash(struct ZirBoolType *type)
{
    const char *type_key = "[bool]";
    unsigned long hash = 5381;
    FlByte c;

    for (size_t i=0; i < strlen(type_key); i++)
        hash = ((hash << 5) + hash) + type_key[i];

    return hash;
}

char* zir_type_bool_to_string(struct ZirBoolType *type)
{
    return "bool";
}

bool zir_type_bool_equals(struct ZirBoolType *type_a, struct ZirType *type_b)
{
    if (type_a == NULL || type_b == NULL)
        return (struct ZirType*) type_a == type_b;

    return type_b->typekind == ZIR_TYPE_BOOL;
}

bool zir_type_bool_is_assignable_from(struct ZirBoolType *target_type, struct ZirType *from_type)
{
    if (target_type == NULL || from_type == NULL)
        return false;

    return from_type->typekind == ZIR_TYPE_BOOL;
}

bool zir_type_bool_is_castable_to(struct ZirBoolType *bool_type, struct ZirType *target_type)
{
    if (target_type == NULL || target_type == NULL)
        return false;

    // It is safe to cast between booleans
    return target_type->typekind == ZIR_TYPE_BOOL;
}

size_t zir_type_bool_size(struct ZirBoolType *type)
{
    if (!type)
        return 0;

    return 1;
}

void zir_type_bool_free(struct ZirBoolType *type)
{
    if (!type)
        return;

    if (type->base.to_string.value != NULL)
        fl_cstring_free(type->base.to_string.value);

    fl_free(type);
}
