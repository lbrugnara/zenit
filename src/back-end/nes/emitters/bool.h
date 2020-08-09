#ifndef ZENIT_NES_EMITTER_BOOL_H
#define ZENIT_NES_EMITTER_BOOL_H

#include <stdlib.h>
#include "../program.h"
#include "../symbols/symbol.h"
#include "../../../zir/instructions/operands/bool.h"

void zenit_nes_emitter_bool_store(ZnesProgram *program, ZirBoolOperand *bool_operand, ZnesSymbol *nes_symbol, size_t offset);

#endif /* ZENIT_NES_EMITTER_BOOL_H */
