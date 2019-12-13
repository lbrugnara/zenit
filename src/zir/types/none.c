#include <fllib.h>
#include <stdlib.h>
#include "none.h"

struct ZirTypeInfo* zir_type_none_new(void)
{
    struct ZirTypeInfo *typeinfo = fl_malloc(sizeof(struct ZirTypeInfo));
    typeinfo->type = ZIR_TYPE_NONE;

    return typeinfo;
}

unsigned long zir_type_none_hash(struct ZirTypeInfo *typeinfo)
{
    unsigned long hash = 5381;
    FlByte c;

    char *type_key = "[none]";
    for (size_t i=0; i < strlen(type_key); i++)
        hash = ((hash << 5) + hash) + type_key[i];

    return hash;
}

void zir_type_none_free(struct ZirTypeInfo *typeinfo)
{
    if (!typeinfo)
        return;

    if (typeinfo->to_string.value != NULL)
        fl_cstring_free(typeinfo->to_string.value);

    fl_free(typeinfo);
}
