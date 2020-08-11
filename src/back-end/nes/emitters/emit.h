#ifndef ZNES_EMITTER_H
#define ZNES_EMITTER_H

#include <stdlib.h>
#include "../program.h"
#include "../symbols/symbol.h"
#include "../../../zir/instructions/operands/operand.h"

void znes_emitter_store(ZnesProgram *program, ZirOperand *operand, ZnesSymbol *nes_symbol, size_t offset);

#endif /* ZNES_EMITTER_H */
