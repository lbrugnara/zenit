#include "emit.h"

#include "array.h"
#include "bool.h"
#include "reference.h"
#include "struct.h"
#include "symbol.h"
#include "uint.h"

void zenit_nes_emitter_store(struct ZenitNesProgram *program, struct ZirOperand *operand, struct ZenitNesSymbol *nes_symbol, size_t offset)
{
    if (operand->type == ZIR_OPERAND_UINT)
    {
        zenit_nes_emitter_uint_store(program, (struct ZirUintOperand*) operand, nes_symbol, 0);
    }
    else if (operand->type == ZIR_OPERAND_BOOL)
    {
        zenit_nes_emitter_bool_store(program, (struct ZirBoolOperand*) operand, nes_symbol, 0);
    }
    else if (operand->type == ZIR_OPERAND_ARRAY)
    {
        zenit_nes_emitter_array_store(program, (struct ZirArrayOperand*) operand, nes_symbol, 0);
    }
    else if (operand->type == ZIR_OPERAND_STRUCT)
    {
        zenit_nes_emitter_struct_store(program, (struct ZirStructOperand*) operand, nes_symbol, 0);
    }
    else if (operand->type == ZIR_OPERAND_REFERENCE)
    {
        zenit_nes_emitter_reference_store(program, (struct ZirReferenceOperand*) operand, nes_symbol, 0);
    }
    else if (operand->type == ZIR_OPERAND_SYMBOL)
    {
        zenit_nes_emitter_symbol_store(program, (struct ZirSymbolOperand*) operand, nes_symbol, 0);
    }
    else
    {
        // FIXME: Add an error
    }
}
