#include <fllib/Cstring.h>
#include <fllib/Array.h>
#include "array.h"
#include "../../../zir/types/array.h"

struct ZenitNesArraySymbol* zenit_nes_symbol_array_new(const char *name, struct ZirArrayType *zir_array_type, enum ZenitNesSegment segment, uint16_t address)
{
    struct ZenitNesArraySymbol *array_symbol = fl_malloc(sizeof(struct ZenitNesArraySymbol));
    array_symbol->base.address = address;
    array_symbol->base.name = name != NULL ? fl_cstring_dup(name) : NULL;
    array_symbol->base.segment = segment;
    array_symbol->base.symkind = ZENIT_NES_SYMBOL_ARRAY;
    array_symbol->base.size = zir_type_array_size(zir_array_type);
    array_symbol->elements = fl_array_new(sizeof(struct ZenitNesSymbol*), zir_array_type->length);

    size_t member_size = zir_type_size(zir_array_type->member_type);

    for (size_t i=0; i < zir_array_type->length; i++)
    {
        char buf[1024] = { 0 };
        size_t to_write = snprintf(NULL, 0, "%zu", i);
        snprintf(buf, 1024, "%zu", i);

        struct ZenitNesSymbol *element = zenit_nes_symbol_new(buf, zir_array_type->member_type, segment, address + (member_size * i));
        array_symbol->elements[i] = element;
    }

    return array_symbol;
}

void zenit_nes_symbol_array_free(struct ZenitNesArraySymbol *array_symbol)
{
    if (array_symbol->base.name)
        fl_cstring_free(array_symbol->base.name);

    fl_array_free_each_pointer(array_symbol->elements, (FlArrayFreeElementFunc) zenit_nes_symbol_free);

    fl_free(array_symbol);
}
