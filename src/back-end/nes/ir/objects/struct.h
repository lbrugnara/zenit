#ifndef ZNES_STRUCT_ALLOC_H
#define ZNES_STRUCT_ALLOC_H

#include <stdint.h>
#include "alloc.h"

typedef struct ZnesStructAlloc {
    ZnesAlloc base;
    ZnesAlloc **members;
} ZnesStructAlloc;

ZnesStructAlloc* znes_struct_alloc_new(const char *name, ZnesSegmentKind segment, size_t size, uint16_t address);
ZnesAlloc* znes_struct_alloc_add_member(ZnesStructAlloc *struct_symbol, ZnesAlloc *member);
void znes_struct_alloc_free(ZnesStructAlloc *struct_symbol);

#endif /* ZNES_STRUCT_ALLOC_H */
