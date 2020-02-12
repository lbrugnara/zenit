#include "temp.h"
#include "emit.h"
#include "../symbols/temp.h"

void zenit_nes_emitter_temp_store(struct ZenitNesProgram *program, struct ZenitNesTempSymbol *temp_symbol, struct ZenitNesSymbol *nes_symbol, size_t offset)
{
    zenit_nes_emitter_store(program, temp_symbol->source, nes_symbol, offset);
}
