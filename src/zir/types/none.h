#ifndef ZIR_TYPE_NONE_H
#define ZIR_TYPE_NONE_H

#include "type.h"

struct ZirType* zir_type_none_new(void);
unsigned long zir_type_none_hash(struct ZirType *type);
void zir_type_none_free(struct ZirType *type);

#endif /* ZIR_TYPE_NONE_H */
