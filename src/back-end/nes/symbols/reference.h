#ifndef ZENIT_NES_REFERENCE_SYMBOL_H
#define ZENIT_NES_REFERENCE_SYMBOL_H

#include <stdint.h>
#include "symbol.h"

struct ZenitNesReferenceSymbol {
    struct ZenitNesSymbol base;
};

struct ZenitNesReferenceSymbol* zenit_nes_symbol_reference_new(const char *name, struct ZirReferenceType *zir_reference_type, enum ZenitNesSegment segment, uint16_t address);
void zenit_nes_symbol_reference_free(struct ZenitNesReferenceSymbol *symbol);

#endif /* ZENIT_NES_REFERENCE_SYMBOL_H */
