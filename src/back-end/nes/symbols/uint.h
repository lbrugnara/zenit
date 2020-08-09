#ifndef ZENIT_NES_UINT_SYMBOL_H
#define ZENIT_NES_UINT_SYMBOL_H

#include <stdint.h>
#include "symbol.h"

typedef struct ZnesUintSymbol {
    ZnesSymbol base;
} ZnesUintSymbol;

ZnesUintSymbol* zenit_nes_symbol_uint_new(const char *name, ZirUintType *zir_uint_type, ZnesSegment segment, uint16_t address);
void zenit_nes_symbol_uint_free(ZnesUintSymbol *symbol);

#endif /* ZENIT_NES_UINT_SYMBOL_H */
