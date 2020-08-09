#ifndef ZENIT_NES_EMITTER_UINT_H
#define ZENIT_NES_EMITTER_UINT_H

#include <stdlib.h>
#include "../program.h"
#include "../symbols/symbol.h"
#include "../../../zir/instructions/operands/uint.h"

void zenit_nes_emitter_uint_store(ZnesProgram *program, ZirUintOperand *uint_operand, ZnesSymbol *nes_symbol, size_t offset);

#endif /* ZENIT_NES_EMITTER_UINT_H */
