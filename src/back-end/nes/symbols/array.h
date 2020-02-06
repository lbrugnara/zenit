#ifndef ZENIT_NES_ARRAY_SYMBOL_H
#define ZENIT_NES_ARRAY_SYMBOL_H

#include <stdint.h>
#include "symbol.h"

struct ZenitNesArraySymbol {
    struct ZenitNesSymbol base;
    struct ZenitNesSymbol **elements;
};

#endif /* ZENIT_NES_ARRAY_SYMBOL_H */
