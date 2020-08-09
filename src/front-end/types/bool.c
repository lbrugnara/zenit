
#include <stdlib.h>
#include <fllib/Cstring.h>
#include "bool.h"

ZenitBoolType* zenit_type_bool_new(void)
{
    ZenitBoolType *type = fl_malloc(sizeof(ZenitBoolType));
    type->base.typekind = ZENIT_TYPE_BOOL;

    return type;
}

unsigned long zenit_type_bool_hash(ZenitBoolType *type)
{
    char *type_key = "bool";

    unsigned long hash = 5381;
    FlByte c;

    for (size_t i=0; i < strlen(type_key); i++)
        hash = ((hash << 5) + hash) + type_key[i];

    return hash;
}

char* zenit_type_bool_to_string(ZenitBoolType *type)
{
    return "bool";
}

bool zenit_type_bool_equals(ZenitBoolType *type_a, ZenitType *type_b)
{
    if (type_a == NULL || type_b == NULL)
        return (ZenitType*) type_a == type_b;

    return type_b->typekind == ZENIT_TYPE_BOOL;
}

bool zenit_type_bool_is_assignable_from(ZenitBoolType *target_type, ZenitType *from_type)
{
    if (target_type == NULL || from_type == NULL)
        return false;

    return from_type->typekind == ZENIT_TYPE_BOOL;
}

bool zenit_type_bool_is_castable_to(ZenitBoolType *bool_type, ZenitType *target_type)
{
    if (target_type == NULL || target_type == NULL)
        return false;

    // It is safe to cast between booleans
    return target_type->typekind == ZENIT_TYPE_BOOL;
}

bool zenit_type_bool_can_unify(ZenitBoolType *bool_type, ZenitType *type_b)
{
    if (bool_type == NULL || type_b == NULL)
        return false;

    if (type_b->typekind != ZENIT_TYPE_NONE && type_b->typekind != ZENIT_TYPE_BOOL)
        return false;

    if (type_b->typekind == ZENIT_TYPE_NONE)
        return true;

    return true;
}

void zenit_type_bool_free(ZenitBoolType *type)
{
    if (!type)
        return;

    if (type->base.to_string.value != NULL)
        fl_cstring_free(type->base.to_string.value);

    fl_free(type);
}
