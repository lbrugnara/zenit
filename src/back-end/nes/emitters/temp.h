#ifndef ZENIT_NES_EMITTER_TEMP_H
#define ZENIT_NES_EMITTER_TEMP_H

#include <stdlib.h>
#include "../program.h"
#include "../symbols/symbol.h"
#include "../symbols/temp.h"
#include "../../../zir/instructions/operands/symbol.h"

void zenit_nes_emitter_temp_store(struct ZenitNesProgram *program, struct ZenitNesTempSymbol *temp_symbol, struct ZenitNesSymbol *nes_symbol, size_t offset);

#endif /* ZENIT_NES_EMITTER_TEMP_H */
