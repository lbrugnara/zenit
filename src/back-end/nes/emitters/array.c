#include "array.h"

#include "emit.h"

#include "../symbols/array.h"
#include "../symbols/temp.h"

void znes_emitter_array_store(ZnesProgram *program, ZirArrayOperand *array, ZnesSymbol *nes_symbol, size_t offset)
{
    if (nes_symbol->symkind != ZNES_SYMBOL_ARRAY)
    {
        // FIXME: Add error handling for this situation
        return;
    }

    // The gap comes from the destination
    ZnesArraySymbol *array_symbol = (ZnesArraySymbol*) nes_symbol;
    size_t gap = array_symbol->base.size / fl_array_length(array_symbol->elements);

    for (size_t i=0; i < fl_array_length(array->elements); i++)
    {
        ZnesSymbol *element_symbol = array_symbol->elements[i];
        ZirOperand *operand = array->elements[i];

        znes_emitter_store(program, operand, element_symbol, 0);
    }
}
