#ifndef ZENIT_NES_STRUCT_SYMBOL_H
#define ZENIT_NES_STRUCT_SYMBOL_H

#include <stdint.h>
#include "symbol.h"

struct ZenitNesStructSymbol {
    struct ZenitNesSymbol base;
    struct ZenitNesSymbol **members;
};

struct ZenitNesStructSymbol* zenit_nes_symbol_struct_new(const char *name, struct ZirStructType *zir_struct_type, enum ZenitNesSegment segment, uint16_t address);
void zenit_nes_symbol_struct_free(struct ZenitNesStructSymbol *struct_symbol);

#endif /* ZENIT_NES_STRUCT_SYMBOL_H */
