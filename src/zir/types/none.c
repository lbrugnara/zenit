
#include <stdlib.h>
#include <string.h>
#include <fllib/Mem.h>
#include <fllib/Cstring.h>
#include "none.h"

ZirType* zir_none_type_new(void)
{
    ZirType *type = fl_malloc(sizeof(ZirType));
    type->typekind = ZIR_TYPE_NONE;

    return type;
}

unsigned long zir_none_type_hash(ZirType *type)
{
    unsigned long hash = 5381;
    FlByte c;

    char *type_key = "[none]";
    for (size_t i=0; i < strlen(type_key); i++)
        hash = ((hash << 5) + hash) + type_key[i];

    return hash;
}

void zir_none_type_free(ZirType *type)
{
    if (!type)
        return;

    if (type->to_string.value != NULL)
        fl_cstring_free(type->to_string.value);

    fl_free(type);
}
