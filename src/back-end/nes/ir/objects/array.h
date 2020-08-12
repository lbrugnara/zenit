#ifndef ZNES_ARRAY_ALLOC_H
#define ZNES_ARRAY_ALLOC_H

#include <stdint.h>
#include "alloc.h"
#include "../operands/operand.h"

typedef struct ZnesArrayAlloc {
    ZnesAlloc base;
    ZnesAlloc **elements;
} ZnesArrayAlloc;

ZnesArrayAlloc* znes_array_alloc_new(const char *name, ZnesSegmentKind segment, size_t size, uint16_t address);
ZnesAlloc* znes_array_alloc_add_element(ZnesArrayAlloc *array_symbol, ZnesAlloc *element);
void znes_array_alloc_free(ZnesArrayAlloc *array_symbol);

#endif /* ZNES_ARRAY_ALLOC_H */
