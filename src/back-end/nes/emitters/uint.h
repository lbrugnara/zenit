#ifndef ZNES_EMITTER_UINT_H
#define ZNES_EMITTER_UINT_H

#include <stdlib.h>
#include "../program.h"
#include "../symbols/symbol.h"
#include "../../../zir/instructions/operands/uint.h"

void znes_emitter_uint_store(ZnesProgram *program, ZirUintOperand *uint_operand, ZnesSymbol *nes_symbol, size_t offset);

#endif /* ZNES_EMITTER_UINT_H */
