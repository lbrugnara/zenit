#ifndef ZENIT_NES_STRUCT_SYMBOL_H
#define ZENIT_NES_STRUCT_SYMBOL_H

#include <stdint.h>
#include "symbol.h"

typedef struct ZnesStructSymbol {
    ZnesSymbol base;
    ZnesSymbol **members;
} ZnesStructSymbol;

ZnesStructSymbol* zenit_nes_symbol_struct_new(const char *name, ZirStructType *zir_struct_type, ZnesSegment segment, uint16_t address);
void zenit_nes_symbol_struct_free(ZnesStructSymbol *struct_symbol);

#endif /* ZENIT_NES_STRUCT_SYMBOL_H */
