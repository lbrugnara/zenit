#ifndef ZENIT_NES_EMITTER_REFERENCE_H
#define ZENIT_NES_EMITTER_REFERENCE_H

#include <stdlib.h>
#include "../program.h"
#include "../symbols/symbol.h"
#include "../../../zir/instructions/operands/reference.h"

void zenit_nes_emitter_reference_store(ZnesProgram *program, ZirReferenceOperand *uint_operand, ZnesSymbol *nes_symbol, size_t offset);

#endif /* ZENIT_NES_EMITTER_REFERENCE_H */
