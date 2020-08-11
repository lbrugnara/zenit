#ifndef ZNES_ARRAY_SYMBOL_H
#define ZNES_ARRAY_SYMBOL_H

#include <stdint.h>
#include "symbol.h"

typedef struct ZnesArraySymbol {
    ZnesSymbol base;
    ZnesSymbol **elements;
} ZnesArraySymbol;

ZnesArraySymbol* znes_symbol_array_new(const char *name, ZirArrayType *zir_array_type, ZnesSegment segment, uint16_t address);
void znes_symbol_array_free(ZnesArraySymbol *array_symbol);

#endif /* ZNES_ARRAY_SYMBOL_H */
