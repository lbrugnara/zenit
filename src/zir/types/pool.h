#ifndef ZIR_TYPE_POOL_H
#define ZIR_TYPE_POOL_H

#include <fllib.h>
#include "system.h"

struct ZirTypePool {
    FlHashtable map;
};

struct ZirTypePool zir_type_pool_new(void);

struct ZirTypeInfo* zir_type_pool_register(struct ZirTypePool *pool, struct ZirTypeInfo *typeinfo);

void zir_type_pool_free(struct ZirTypePool *pool);

struct ZirArrayTypeInfo* zir_type_pool_register_array(struct ZirTypePool *pool, struct ZirArrayTypeInfo *typeinfo);
    
struct ZirUintTypeInfo* zir_type_pool_register_primitive(struct ZirTypePool *pool, struct ZirUintTypeInfo *typeinfo);

struct ZirReferenceTypeInfo* zir_type_pool_register_reference(struct ZirTypePool *pool, struct ZirReferenceTypeInfo *typeinfo);

struct ZirStructTypeInfo* zir_type_pool_register_struct(struct ZirTypePool *pool, struct ZirStructTypeInfo *typeinfo);

#endif /* ZIR_TYPE_POOL_H */
