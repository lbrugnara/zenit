#ifndef ZENIT_NES_ARRAY_SYMBOL_H
#define ZENIT_NES_ARRAY_SYMBOL_H

#include <stdint.h>
#include "symbol.h"

typedef struct ZnesArraySymbol {
    ZnesSymbol base;
    ZnesSymbol **elements;
} ZnesArraySymbol;

ZnesArraySymbol* zenit_nes_symbol_array_new(const char *name, ZirArrayType *zir_array_type, ZnesSegment segment, uint16_t address);
void zenit_nes_symbol_array_free(ZnesArraySymbol *array_symbol);

#endif /* ZENIT_NES_ARRAY_SYMBOL_H */
