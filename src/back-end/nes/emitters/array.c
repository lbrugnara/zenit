#include "array.h"

#include "reference.h"
#include "struct.h"
#include "symbol.h"
#include "temp.h"
#include "uint.h"

#include "../symbols/array.h"
#include "../symbols/reference.h"
#include "../symbols/struct.h"
#include "../symbols/temp.h"
#include "../symbols/uint.h"

void zenit_nes_emitter_array_store(struct ZenitNesProgram *program, struct ZirArrayOperand *array, struct ZenitNesSymbol *nes_symbol, size_t offset)
{
    // The store array on a temp symbol is just the assignment of the array operand to the "source" property of the temp symbol
    if (nes_symbol->symkind == ZENIT_NES_SYMBOL_TEMP)
    {
        ((struct ZenitNesTempSymbol*) nes_symbol)->source = (struct ZirOperand*) array;
        return;
    }
    else if (nes_symbol->symkind != ZENIT_NES_SYMBOL_ARRAY)
    {
        // FIXME: Add error handling for this situation
        return;
    }

    // The gap comes from the destination
    struct ZenitNesArraySymbol *array_symbol = (struct ZenitNesArraySymbol*) nes_symbol;
    size_t gap = array_symbol->base.size / fl_array_length(array_symbol->elements);

    for (size_t i=0; i < fl_array_length(array->elements); i++)
    {
        struct ZenitNesSymbol *element_symbol = array_symbol->elements[i];
        struct ZirOperand *operand = array->elements[i];
        
        if (operand->type == ZIR_OPERAND_UINT)
        {
            zenit_nes_emitter_uint_store(program, (struct ZirUintOperand*) operand, element_symbol, 0);
        }
        else if (operand->type == ZIR_OPERAND_ARRAY)
        {
            zenit_nes_emitter_array_store(program, (struct ZirArrayOperand*) operand, element_symbol, 0);
        }
        else if (operand->type == ZIR_OPERAND_STRUCT)
        {
            zenit_nes_emitter_struct_store(program, (struct ZirStructOperand*) operand, element_symbol, 0);
        }
        else if (operand->type == ZIR_OPERAND_REFERENCE)
        {
            zenit_nes_emitter_reference_store(program, (struct ZirReferenceOperand*) operand, element_symbol, 0);
        }
        else if (operand->type == ZIR_OPERAND_SYMBOL)
        {
            zenit_nes_emitter_symbol_store(program, (struct ZirSymbolOperand*) operand, element_symbol, 0);
        }
    }
}
