#ifndef ZNES_REFERENCE_SYMBOL_H
#define ZNES_REFERENCE_SYMBOL_H

#include <stdint.h>
#include "symbol.h"

typedef struct ZnesReferenceSymbol {
    ZnesSymbol base;
} ZnesReferenceSymbol;

ZnesReferenceSymbol* znes_symbol_reference_new(const char *name, ZirReferenceType *zir_reference_type, ZnesSegment segment, uint16_t address);
void znes_symbol_reference_free(ZnesReferenceSymbol *symbol);

#endif /* ZNES_REFERENCE_SYMBOL_H */
