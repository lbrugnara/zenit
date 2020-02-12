#ifndef ZENIT_NES_EMITTER_BOOL_H
#define ZENIT_NES_EMITTER_BOOL_H

#include <stdlib.h>
#include "../program.h"
#include "../symbols/symbol.h"
#include "../../../zir/instructions/operands/bool.h"

void zenit_nes_emitter_bool_store(struct ZenitNesProgram *program, struct ZirBoolOperand *bool_operand, struct ZenitNesSymbol *nes_symbol, size_t offset);

#endif /* ZENIT_NES_EMITTER_BOOL_H */
