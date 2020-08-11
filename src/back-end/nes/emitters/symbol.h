#ifndef ZNES_EMITTER_SYMBOL_H
#define ZNES_EMITTER_SYMBOL_H

#include <stdlib.h>
#include "../program.h"
#include "../symbols/symbol.h"
#include "../../../zir/instructions/operands/symbol.h"

void znes_emitter_symbol_store(ZnesProgram *program, ZirSymbolOperand *symbol_operand, ZnesSymbol *nes_symbol, size_t offset);

#endif /* ZNES_EMITTER_SYMBOL_H */
