#include <fllib/Cstring.h>
#include <fllib/Array.h>
#include "array.h"
#include "../../../zir/types/array.h"

ZnesArraySymbol* zenit_nes_symbol_array_new(const char *name, ZirArrayType *zir_array_type, ZnesSegment segment, uint16_t address)
{
    ZnesArraySymbol *array_symbol = fl_malloc(sizeof(ZnesArraySymbol));
    array_symbol->base.address = address;
    array_symbol->base.name = name != NULL ? fl_cstring_dup(name) : NULL;
    array_symbol->base.segment = segment;
    array_symbol->base.symkind = ZENIT_NES_SYMBOL_ARRAY;
    array_symbol->base.size = zir_type_array_size(zir_array_type);
    array_symbol->elements = fl_array_new(sizeof(ZnesSymbol*), zir_array_type->length);

    size_t member_size = zir_type_size(zir_array_type->member_type);

    for (size_t i=0; i < zir_array_type->length; i++)
    {
        char buf[1024] = { 0 };
        size_t to_write = snprintf(NULL, 0, "%zu", i);
        snprintf(buf, 1024, "%zu", i);

        ZnesSymbol *element = zenit_nes_symbol_new(buf, zir_array_type->member_type, segment, address + (member_size * i));
        array_symbol->elements[i] = element;
    }

    return array_symbol;
}

void zenit_nes_symbol_array_free(ZnesArraySymbol *array_symbol)
{
    if (array_symbol->base.name)
        fl_cstring_free(array_symbol->base.name);

    fl_array_free_each_pointer(array_symbol->elements, (FlArrayFreeElementFunc) zenit_nes_symbol_free);

    fl_free(array_symbol);
}
