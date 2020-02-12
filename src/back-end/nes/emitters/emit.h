#ifndef ZENIT_NES_EMITTER_H
#define ZENIT_NES_EMITTER_H

#include <stdlib.h>
#include "../program.h"
#include "../symbols/symbol.h"
#include "../../../zir/instructions/operands/operand.h"

void zenit_nes_emitter_store(struct ZenitNesProgram *program, struct ZirOperand *operand, struct ZenitNesSymbol *nes_symbol, size_t offset);

#endif /* ZENIT_NES_EMITTER_H */
