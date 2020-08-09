#ifndef ZENIT_NES_TEMP_SYMBOL_H
#define ZENIT_NES_TEMP_SYMBOL_H

#include <stdint.h>
#include "symbol.h"
#include "../../../zir/instructions/operands/operand.h"

typedef struct ZnesTempSymbol {
    ZnesSymbol base;
    ZirOperand *source;
} ZnesTempSymbol;

ZnesTempSymbol* zenit_nes_symbol_temp_new(const char *name, ZirType *zir_type);
void zenit_nes_symbol_temp_free(ZnesTempSymbol *symbol);

#endif /* ZENIT_NES_TEMP_SYMBOL_H */
