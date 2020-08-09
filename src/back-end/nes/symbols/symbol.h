#ifndef ZENIT_NES_SYMBOL
#define ZENIT_NES_SYMBOL

#include <stdint.h>
#include "../segment.h"
#include "../../../zir/types/system.h"

typedef enum ZnesSymbolKind {
    ZENIT_NES_SYMBOL_UINT,
    ZENIT_NES_SYMBOL_BOOL,
    ZENIT_NES_SYMBOL_ARRAY,
    ZENIT_NES_SYMBOL_STRUCT,
    ZENIT_NES_SYMBOL_REFERENCE,
    ZENIT_NES_SYMBOL_TEMP,
} ZnesSymbolKind;

typedef struct ZnesSymbol {
    ZnesSymbolKind symkind;
    ZnesSegment segment;
    char *name;
    uint16_t address;
    size_t size;
} ZnesSymbol;

ZnesSymbol* zenit_nes_symbol_new(const char *name, ZirType *zir_type, ZnesSegment segment, uint16_t address);
void zenit_nes_symbol_free(ZnesSymbol *symbol);

#endif /* ZENIT_NES_SYMBOL */
