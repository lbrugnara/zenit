#ifndef ZENIT_NES_BOOL_SYMBOL_H
#define ZENIT_NES_BOOL_SYMBOL_H

#include <stdint.h>
#include "symbol.h"

struct ZenitNesBoolSymbol {
    struct ZenitNesSymbol base;
};

struct ZenitNesBoolSymbol* zenit_nes_symbol_bool_new(const char *name, struct ZirBoolType *zir_bool_type, enum ZenitNesSegment segment, uint16_t address);
void zenit_nes_symbol_bool_free(struct ZenitNesBoolSymbol *symbol);

#endif /* ZENIT_NES_BOOL_SYMBOL_H */
