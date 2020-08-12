#ifndef ZNES_UINT_ALLOC_H
#define ZNES_UINT_ALLOC_H

#include <stdint.h>
#include "alloc.h"

typedef struct ZnesUintAlloc {
    ZnesAlloc base;
} ZnesUintAlloc;

ZnesUintAlloc* znes_uint_alloc_new(const char *name, ZnesSegmentKind segment, size_t size, uint16_t address);
void znes_uint_alloc_free(ZnesUintAlloc *symbol);

#endif /* ZNES_UINT_ALLOC_H */
