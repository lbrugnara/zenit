#include "temp.h"
#include "emit.h"
#include "../symbols/temp.h"

void znes_emitter_temp_store(ZnesProgram *program, ZnesTempSymbol *temp_symbol, ZnesSymbol *nes_symbol, size_t offset)
{
    znes_emitter_store(program, temp_symbol->source, nes_symbol, offset);
}
