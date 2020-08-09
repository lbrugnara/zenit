#ifndef ZENIT_NES_EMITTER_STRUCT_H
#define ZENIT_NES_EMITTER_STRUCT_H

#include <stdlib.h>
#include "../program.h"
#include "../symbols/symbol.h"
#include "../../../zir/instructions/operands/struct.h"

void zenit_nes_emitter_struct_store(ZnesProgram *program, ZirStructOperand *struct_operand, ZnesSymbol *nes_symbol, size_t offset);

#endif /* ZENIT_NES_EMITTER_STRUCT_H */
