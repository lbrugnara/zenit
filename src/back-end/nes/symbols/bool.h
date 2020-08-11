#ifndef ZNES_BOOL_SYMBOL_H
#define ZNES_BOOL_SYMBOL_H

#include <stdint.h>
#include "symbol.h"

typedef struct ZnesBoolSymbol {
    ZnesSymbol base;
} ZnesBoolSymbol;

ZnesBoolSymbol* znes_bool_symbol_new(const char *name, ZirBoolType *zir_bool_type, ZnesSegment segment, uint16_t address);
void znes_bool_symbol_free(ZnesBoolSymbol *symbol);

#endif /* ZNES_BOOL_SYMBOL_H */
