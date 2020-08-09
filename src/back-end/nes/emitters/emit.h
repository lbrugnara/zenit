#ifndef ZENIT_NES_EMITTER_H
#define ZENIT_NES_EMITTER_H

#include <stdlib.h>
#include "../program.h"
#include "../symbols/symbol.h"
#include "../../../zir/instructions/operands/operand.h"

void zenit_nes_emitter_store(ZnesProgram *program, ZirOperand *operand, ZnesSymbol *nes_symbol, size_t offset);

#endif /* ZENIT_NES_EMITTER_H */
