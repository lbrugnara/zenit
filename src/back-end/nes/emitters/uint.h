#ifndef ZENIT_NES_EMITTER_UINT_H
#define ZENIT_NES_EMITTER_UINT_H

#include <stdlib.h>
#include "../program.h"
#include "../symbols/symbol.h"
#include "../../../zir/instructions/operands/uint.h"

void zenit_nes_emitter_uint_store(struct ZenitNesProgram *program, struct ZirUintOperand *uint_operand, struct ZenitNesSymbol *nes_symbol, size_t offset);

#endif /* ZENIT_NES_EMITTER_UINT_H */
