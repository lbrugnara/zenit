#ifndef ZENIT_TYPE_POOL_H
#define ZENIT_TYPE_POOL_H

#include <fllib.h>
#include "type.h"

typedef FlList ZenitTypePool;

static inline ZenitTypePool zenit_type_pool_new(void)
{
    return fl_list_new_args((struct FlListArgs) {
        .value_cleaner = (FlContainerCleanupFunction) zenit_type_free
    });
}

static inline struct ZenitTypeInfo* zenit_type_pool_register(ZenitTypePool pool, struct ZenitTypeInfo *typeinfo)
{
    struct FlListNode *node = fl_list_append(pool, typeinfo);

    if (!node)
        return NULL;

    return (struct ZenitTypeInfo*) node->value;
}

static inline void zenit_type_pool_free(ZenitTypePool pool)
{
    fl_list_free(pool);
}

#endif /* ZENIT_TYPE_POOL_H */
