#ifndef ZENIT_NES_ARRAY_SYMBOL_H
#define ZENIT_NES_ARRAY_SYMBOL_H

#include <stdint.h>
#include "symbol.h"

struct ZenitNesArraySymbol {
    struct ZenitNesSymbol base;
    struct ZenitNesSymbol **elements;
};

struct ZenitNesArraySymbol* zenit_nes_symbol_array_new(const char *name, struct ZirArrayType *zir_array_type, enum ZenitNesSegment segment, uint16_t address);
void zenit_nes_symbol_array_free(struct ZenitNesArraySymbol *array_symbol);

#endif /* ZENIT_NES_ARRAY_SYMBOL_H */
