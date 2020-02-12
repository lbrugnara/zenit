#ifndef ZENIT_NES_UINT_SYMBOL_H
#define ZENIT_NES_UINT_SYMBOL_H

#include <stdint.h>
#include "symbol.h"

struct ZenitNesUintSymbol {
    struct ZenitNesSymbol base;
};

struct ZenitNesUintSymbol* zenit_nes_symbol_uint_new(const char *name, struct ZirUintType *zir_uint_type, enum ZenitNesSegment segment, uint16_t address);
void zenit_nes_symbol_uint_free(struct ZenitNesUintSymbol *symbol);

#endif /* ZENIT_NES_UINT_SYMBOL_H */
