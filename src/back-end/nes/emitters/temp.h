#ifndef ZNES_EMITTER_TEMP_H
#define ZNES_EMITTER_TEMP_H

#include <stdlib.h>
#include "../program.h"
#include "../symbols/symbol.h"
#include "../symbols/temp.h"
#include "../../../zir/instructions/operands/symbol.h"

void znes_emitter_temp_store(ZnesProgram *program, ZnesTempSymbol *temp_symbol, ZnesSymbol *nes_symbol, size_t offset);

#endif /* ZNES_EMITTER_TEMP_H */
