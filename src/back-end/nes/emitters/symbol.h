#ifndef ZENIT_NES_EMITTER_SYMBOL_H
#define ZENIT_NES_EMITTER_SYMBOL_H

#include <stdlib.h>
#include "../program.h"
#include "../symbols/symbol.h"
#include "../../../zir/instructions/operands/symbol.h"

void zenit_nes_emitter_symbol_store(struct ZenitNesProgram *program, struct ZirSymbolOperand *symbol_operand, struct ZenitNesSymbol *nes_symbol, size_t offset);

#endif /* ZENIT_NES_EMITTER_SYMBOL_H */
