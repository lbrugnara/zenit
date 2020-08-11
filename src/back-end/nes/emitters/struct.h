#ifndef ZNES_EMITTER_STRUCT_H
#define ZNES_EMITTER_STRUCT_H

#include <stdlib.h>
#include "../program.h"
#include "../symbols/symbol.h"
#include "../../../zir/instructions/operands/struct.h"

void znes_emitter_struct_store(ZnesProgram *program, ZirStructOperand *struct_operand, ZnesSymbol *nes_symbol, size_t offset);

#endif /* ZNES_EMITTER_STRUCT_H */
