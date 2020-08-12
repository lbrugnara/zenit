#ifndef ZNES_REFERENCE_ALLOC_H
#define ZNES_REFERENCE_ALLOC_H

#include <stdint.h>
#include "alloc.h"

typedef struct ZnesReferenceAlloc {
    ZnesAlloc base;
} ZnesReferenceAlloc;

ZnesReferenceAlloc* znes_reference_alloc_new(const char *name, ZnesSegmentKind segment, size_t size, uint16_t address);
void znes_reference_alloc_free(ZnesReferenceAlloc *symbol);

#endif /* ZNES_REFERENCE_ALLOC_H */
