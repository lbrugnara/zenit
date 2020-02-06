#ifndef ZENIT_NES_TEMP_SYMBOL_H
#define ZENIT_NES_TEMP_SYMBOL_H

#include <stdint.h>
#include "symbol.h"

struct ZenitNesTempSymbol {
    struct ZenitNesSymbol base;
    struct ZirOperand *source;
};

#endif /* ZENIT_NES_TEMP_SYMBOL_H */
