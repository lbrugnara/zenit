#include "array.h"

#include "emit.h"

#include "../symbols/array.h"
#include "../symbols/temp.h"

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

        zenit_nes_emitter_store(program, operand, element_symbol, 0);
    }
}
