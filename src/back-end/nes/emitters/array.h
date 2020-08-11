#ifndef ZNES_EMITTER_ARRAY_H
#define ZNES_EMITTER_ARRAY_H

#include <stdlib.h>
#include "../program.h"
#include "../symbols/symbol.h"
#include "../../../zir/instructions/operands/array.h"

void znes_emitter_array_store(ZnesProgram *program, ZirArrayOperand *array, ZnesSymbol *nes_symbol, size_t offset);

#endif /* ZNES_EMITTER_ARRAY_H */
