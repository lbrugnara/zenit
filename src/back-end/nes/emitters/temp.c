#include "temp.h"

#include "array.h"
#include "reference.h"
#include "struct.h"
#include "symbol.h"
#include "uint.h"

#include "../symbols/array.h"
#include "../symbols/reference.h"
#include "../symbols/struct.h"
#include "../symbols/temp.h"
#include "../symbols/uint.h"

void zenit_nes_emitter_temp_store(struct ZenitNesProgram *program, struct ZenitNesTempSymbol *temp_symbol, struct ZenitNesSymbol *nes_symbol, size_t offset)
{
    if (temp_symbol->source->type == ZIR_OPERAND_ARRAY)
    {
        size_t array_size = zir_type_size(((struct ZirArrayOperand*) temp_symbol->source)->type->member_type);
        size_t gap = nes_symbol->size > array_size ? array_size : nes_symbol->size;
        zenit_nes_emitter_array_store(program, (struct ZirArrayOperand*) temp_symbol->source, nes_symbol, offset);
    }
    else if (temp_symbol->source->type == ZIR_OPERAND_STRUCT)
    {
        zenit_nes_emitter_struct_store(program, (struct ZirStructOperand*) temp_symbol->source, nes_symbol, offset);
    }
    else if (temp_symbol->source->type == ZIR_OPERAND_REFERENCE)
    {
        zenit_nes_emitter_reference_store(program, (struct ZirReferenceOperand*) temp_symbol->source, nes_symbol, offset);
    }
    else if (temp_symbol->source->type == ZIR_OPERAND_SYMBOL)
    {
        zenit_nes_emitter_symbol_store(program, (struct ZirSymbolOperand*) temp_symbol->source, nes_symbol, offset);
    }
    else if (temp_symbol->source->type == ZIR_OPERAND_UINT)
    {
        zenit_nes_emitter_uint_store(program, (struct ZirUintOperand*) temp_symbol->source, nes_symbol, offset);
    }
}
