#ifndef ZENIT_NES_SYMBOL
#define ZENIT_NES_SYMBOL

#include <stdint.h>
#include "../segment.h"

enum ZenitNesSymbolKind {
    ZENIT_NES_SYMBOL_UINT,
    ZENIT_NES_SYMBOL_ARRAY,
    ZENIT_NES_SYMBOL_STRUCT,
    ZENIT_NES_SYMBOL_REFERENCE,
    ZENIT_NES_SYMBOL_TEMP,
};

struct ZenitNesSymbol {
    enum ZenitNesSymbolKind symkind;
    enum ZenitNesSegment segment;
    char *name;
    uint16_t address;
    size_t size;
};

#endif /* ZENIT_NES_SYMBOL */
