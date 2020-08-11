#ifndef ZNES_EMITTER_BOOL_H
#define ZNES_EMITTER_BOOL_H

#include <stdlib.h>
#include "../program.h"
#include "../symbols/symbol.h"
#include "../../../zir/instructions/operands/bool.h"

void znes_emitter_bool_store(ZnesProgram *program, ZirBoolOperand *bool_operand, ZnesSymbol *nes_symbol, size_t offset);

#endif /* ZNES_EMITTER_BOOL_H */
