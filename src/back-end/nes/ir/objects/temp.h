#ifndef ZNES_TEMP_ALLOC_H
#define ZNES_TEMP_ALLOC_H

#include <stdint.h>
#include "alloc.h"
#include "../operands/operand.h"

typedef struct ZnesTempAlloc {
    ZnesAlloc base;
    ZnesOperand *source;
} ZnesTempAlloc;

ZnesTempAlloc* znes_temp_alloc_new(const char *name, size_t size);
void znes_temp_alloc_free(ZnesTempAlloc *symbol);

#endif /* ZNES_TEMP_ALLOC_H */
