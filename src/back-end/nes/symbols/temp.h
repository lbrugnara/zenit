#ifndef ZENIT_NES_TEMP_SYMBOL_H
#define ZENIT_NES_TEMP_SYMBOL_H

#include <stdint.h>
#include "symbol.h"

struct ZenitNesTempSymbol {
    struct ZenitNesSymbol base;
    struct ZirOperand *source;
};

struct ZenitNesTempSymbol* zenit_nes_symbol_temp_new(const char *name, struct ZirType *zir_type);
void zenit_nes_symbol_temp_free(struct ZenitNesTempSymbol *symbol);

#endif /* ZENIT_NES_TEMP_SYMBOL_H */
