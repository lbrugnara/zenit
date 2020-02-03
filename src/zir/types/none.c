
#include <stdlib.h>
#include <string.h>
#include <fllib/Mem.h>
#include <fllib/Cstring.h>
#include "none.h"

struct ZirType* zir_type_none_new(void)
{
    struct ZirType *type = fl_malloc(sizeof(struct ZirType));
    type->typekind = ZIR_TYPE_NONE;

    return type;
}

unsigned long zir_type_none_hash(struct ZirType *type)
{
    unsigned long hash = 5381;
    FlByte c;

    char *type_key = "[none]";
    for (size_t i=0; i < strlen(type_key); i++)
        hash = ((hash << 5) + hash) + type_key[i];

    return hash;
}

void zir_type_none_free(struct ZirType *type)
{
    if (!type)
        return;

    if (type->to_string.value != NULL)
        fl_cstring_free(type->to_string.value);

    fl_free(type);
}
