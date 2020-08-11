#ifndef ZNES_UINT_SYMBOL_H
#define ZNES_UINT_SYMBOL_H

#include <stdint.h>
#include "symbol.h"

typedef struct ZnesUintSymbol {
    ZnesSymbol base;
} ZnesUintSymbol;

ZnesUintSymbol* znes_symbol_uint_new(const char *name, ZirUintType *zir_uint_type, ZnesSegment segment, uint16_t address);
void znes_symbol_uint_free(ZnesUintSymbol *symbol);

#endif /* ZNES_UINT_SYMBOL_H */
