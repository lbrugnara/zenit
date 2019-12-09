#ifndef ZENIT_TYPE_NONE_H
#define ZENIT_TYPE_NONE_H

#include "type.h"

struct ZenitTypeInfo* zenit_type_none_new(void);
unsigned long zenit_type_none_hash(struct ZenitTypeInfo *typeinfo);
void zenit_type_none_free(struct ZenitTypeInfo *typeinfo);

#endif /* ZENIT_TYPE_NONE_H */
