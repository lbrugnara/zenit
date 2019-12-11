#ifndef ZENIT_TYPE_POOL_H
#define ZENIT_TYPE_POOL_H

#include <fllib.h>
#include "system.h"

struct ZenitTypePool {
    FlHashtable map;
};

struct ZenitTypePool zenit_type_pool_new(void);

struct ZenitTypeInfo* zenit_type_pool_register(struct ZenitTypePool *pool, struct ZenitTypeInfo *typeinfo);

void zenit_type_pool_free(struct ZenitTypePool *pool);

struct ZenitArrayTypeInfo* zenit_type_pool_register_array(struct ZenitTypePool *pool, struct ZenitArrayTypeInfo *typeinfo);
    
struct ZenitUintTypeInfo* zenit_type_pool_register_primitive(struct ZenitTypePool *pool, struct ZenitUintTypeInfo *typeinfo);

struct ZenitReferenceTypeInfo* zenit_type_pool_register_reference(struct ZenitTypePool *pool, struct ZenitReferenceTypeInfo *typeinfo);

struct ZenitStructTypeInfo* zenit_type_pool_register_struct(struct ZenitTypePool *pool, struct ZenitStructTypeInfo *typeinfo);

#endif /* ZENIT_TYPE_POOL_H */
