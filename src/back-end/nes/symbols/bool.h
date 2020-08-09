#ifndef ZENIT_NES_BOOL_SYMBOL_H
#define ZENIT_NES_BOOL_SYMBOL_H

#include <stdint.h>
#include "symbol.h"

typedef struct ZnesBoolSymbol {
    ZnesSymbol base;
} ZnesBoolSymbol;

ZnesBoolSymbol* zenit_nes_symbol_bool_new(const char *name, ZirBoolType *zir_bool_type, ZnesSegment segment, uint16_t address);
void zenit_nes_symbol_bool_free(ZnesBoolSymbol *symbol);

#endif /* ZENIT_NES_BOOL_SYMBOL_H */
