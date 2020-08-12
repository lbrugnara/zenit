#ifndef ZNES_BOOL_ALLOC_H
#define ZNES_BOOL_ALLOC_H

#include <stdint.h>
#include "alloc.h"

typedef struct ZnesBoolAlloc {
    ZnesAlloc base;
} ZnesBoolAlloc;

ZnesBoolAlloc* znes_bool_alloc_new(const char *name, ZnesSegmentKind segment, size_t size, uint16_t address);
void znes_bool_alloc_free(ZnesBoolAlloc *symbol);

#endif /* ZNES_BOOL_ALLOC_H */
