#ifndef ZENIT_NES_REFERENCE_SYMBOL_H
#define ZENIT_NES_REFERENCE_SYMBOL_H

#include <stdint.h>
#include "symbol.h"

typedef struct ZnesReferenceSymbol {
    ZnesSymbol base;
} ZnesReferenceSymbol;

ZnesReferenceSymbol* zenit_nes_symbol_reference_new(const char *name, ZirReferenceType *zir_reference_type, ZnesSegment segment, uint16_t address);
void zenit_nes_symbol_reference_free(ZnesReferenceSymbol *symbol);

#endif /* ZENIT_NES_REFERENCE_SYMBOL_H */
