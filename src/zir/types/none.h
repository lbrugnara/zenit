#ifndef ZIR_TYPE_NONE_H
#define ZIR_TYPE_NONE_H

#include "type.h"

struct ZirTypeInfo* zir_type_none_new(void);
unsigned long zir_type_none_hash(struct ZirTypeInfo *typeinfo);
void zir_type_none_free(struct ZirTypeInfo *typeinfo);

#endif /* ZIR_TYPE_NONE_H */
