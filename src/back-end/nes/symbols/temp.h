#ifndef ZNES_TEMP_SYMBOL_H
#define ZNES_TEMP_SYMBOL_H

#include <stdint.h>
#include "symbol.h"
#include "../../../zir/instructions/operands/operand.h"

typedef struct ZnesTempSymbol {
    ZnesSymbol base;
    ZirOperand *source;
} ZnesTempSymbol;

ZnesTempSymbol* znes_temp_symbol_new(const char *name, ZirType *zir_type);
void znes_temp_symbol_free(ZnesTempSymbol *symbol);

#endif /* ZNES_TEMP_SYMBOL_H */
