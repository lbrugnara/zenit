#ifndef ZENIT_NES_STRUCT_SYMBOL_H
#define ZENIT_NES_STRUCT_SYMBOL_H

#include <stdint.h>
#include "symbol.h"

struct ZenitNesStructSymbol {
    struct ZenitNesSymbol base;
    struct ZenitNesSymbol **members;
};

#endif /* ZENIT_NES_STRUCT_SYMBOL_H */
