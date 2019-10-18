#ifndef CENIT_SYMBOL_H
#define CENIT_SYMBOL_H

#include <fllib.h>
#include "type.h"

typedef struct CenitSymbol {
    const char *name;
    CenitTypeInfo typeinfo;
} CenitSymbol;

CenitSymbol* cenit_symbol_new(const char *name, CenitTypeInfo *typeinfo);
void cenit_symbol_free(CenitSymbol *symbol);

#endif /* CENIT_SYMBOL_H */
