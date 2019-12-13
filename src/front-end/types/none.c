#include <fllib.h>
#include <stdlib.h>
#include "none.h"

struct ZenitTypeInfo* zenit_type_none_new(void)
{
    struct ZenitTypeInfo *typeinfo = fl_malloc(sizeof(struct ZenitTypeInfo));
    typeinfo->type = ZENIT_TYPE_NONE;
    typeinfo->source = ZENIT_TYPE_SRC_INFERRED;

    return typeinfo;
}

unsigned long zenit_type_none_hash(struct ZenitTypeInfo *typeinfo)
{
    unsigned long hash = 5381;
    FlByte c;

    char *type_key = "[none]";
    for (size_t i=0; i < strlen(type_key); i++)
        hash = ((hash << 5) + hash) + type_key[i];

    return hash;
}

void zenit_type_none_free(struct ZenitTypeInfo *typeinfo)
{
    if (!typeinfo)
        return;

    if (typeinfo->to_string.value != NULL)
        fl_cstring_free(typeinfo->to_string.value);

    fl_free(typeinfo);
}
