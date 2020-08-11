#ifndef ZNES_SYMBOL
#define ZNES_SYMBOL

#include <stdint.h>
#include "../segment.h"
#include "../../../zir/types/system.h"

typedef enum ZnesSymbolKind {
    ZNES_SYMBOL_UINT,
    ZNES_SYMBOL_BOOL,
    ZNES_SYMBOL_ARRAY,
    ZNES_SYMBOL_STRUCT,
    ZNES_SYMBOL_REFERENCE,
    ZNES_SYMBOL_TEMP,
} ZnesSymbolKind;

typedef struct ZnesSymbol {
    ZnesSymbolKind symkind;
    ZnesSegment segment;
    char *name;
    uint16_t address;
    size_t size;
} ZnesSymbol;

ZnesSymbol* znes_symbol_new(const char *name, ZirType *zir_type, ZnesSegment segment, uint16_t address);
void znes_symbol_free(ZnesSymbol *symbol);

#endif /* ZNES_SYMBOL */
