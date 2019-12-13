#ifndef ZIR_OPERAND_SYMBOL_H
#define ZIR_OPERAND_SYMBOL_H

#include "operand.h"
#include "../../symbol.h"

struct ZirSymbolOperand {
    struct ZirOperand base;
    struct ZirSymbol *symbol;
};

struct ZirSymbolOperand* zir_operand_symbol_new(struct ZirSymbol *symbol);
void zir_operand_symbol_free(struct ZirSymbolOperand *operand);
char* zir_operand_symbol_dump(struct ZirSymbolOperand *operand, char *output);
char* zir_operand_symbol_type_dump(struct ZirSymbolOperand *operand, char *output);

#endif /* ZIR_OPERAND_SYMBOL_H */
