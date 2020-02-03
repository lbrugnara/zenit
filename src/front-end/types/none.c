
#include <stdlib.h>
#include <fllib/Cstring.h>
#include "none.h"

struct ZenitType* zenit_type_none_new(void)
{
    struct ZenitType *type = fl_malloc(sizeof(struct ZenitType));
    type->typekind = ZENIT_TYPE_NONE;

    return type;
}

unsigned long zenit_type_none_hash(struct ZenitType *type)
{
    unsigned long hash = 5381;
    FlByte c;

    char *type_key = "[none]";
    for (size_t i=0; i < strlen(type_key); i++)
        hash = ((hash << 5) + hash) + type_key[i];

    return hash;
}

void zenit_type_none_free(struct ZenitType *type)
{
    if (!type)
        return;

    if (type->to_string.value != NULL)
        fl_cstring_free(type->to_string.value);

    fl_free(type);
}
