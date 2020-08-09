#include "emit.h"
#include "array.h"
#include "bool.h"
#include "reference.h"
#include "struct.h"
#include "symbol.h"
#include "uint.h"

#include "../symbols/temp.h"

void zenit_nes_emitter_store(ZnesProgram *program, ZirOperand *operand, ZnesSymbol *nes_symbol, size_t offset)
{
    if (nes_symbol->symkind == ZENIT_NES_SYMBOL_TEMP)
    {
        ((ZnesTempSymbol*) nes_symbol)->source = operand;
        return;
    }

    if (operand->type == ZIR_OPERAND_UINT)
    {
        zenit_nes_emitter_uint_store(program, (ZirUintOperand*) operand, nes_symbol, 0);
    }
    else if (operand->type == ZIR_OPERAND_BOOL)
    {
        zenit_nes_emitter_bool_store(program, (ZirBoolOperand*) operand, nes_symbol, 0);
    }
    else if (operand->type == ZIR_OPERAND_ARRAY)
    {
        zenit_nes_emitter_array_store(program, (ZirArrayOperand*) operand, nes_symbol, 0);
    }
    else if (operand->type == ZIR_OPERAND_STRUCT)
    {
        zenit_nes_emitter_struct_store(program, (ZirStructOperand*) operand, nes_symbol, 0);
    }
    else if (operand->type == ZIR_OPERAND_REFERENCE)
    {
        zenit_nes_emitter_reference_store(program, (ZirReferenceOperand*) operand, nes_symbol, 0);
    }
    else if (operand->type == ZIR_OPERAND_SYMBOL)
    {
        zenit_nes_emitter_symbol_store(program, (ZirSymbolOperand*) operand, nes_symbol, 0);
    }
    else
    {
        // FIXME: Add an error
    }
}
