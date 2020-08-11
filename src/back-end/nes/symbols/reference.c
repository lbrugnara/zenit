#include <fllib/Cstring.h>
#include "reference.h"
#include "../../../zir/types/reference.h"

ZnesReferenceSymbol* znes_reference_symbol_new(const char *name, ZirReferenceType *zir_reference_type, ZnesSegment segment, uint16_t address)
{
    ZnesReferenceSymbol *ref_symbol = fl_malloc(sizeof(ZnesReferenceSymbol));
    ref_symbol->base.address = address;
    ref_symbol->base.name = name != NULL ? fl_cstring_dup(name) : NULL;
    ref_symbol->base.segment = segment;
    ref_symbol->base.symkind = ZNES_SYMBOL_REFERENCE;
    ref_symbol->base.size = zir_reference_type_size(zir_reference_type);

    return ref_symbol;
}

void znes_reference_symbol_free(ZnesReferenceSymbol *symbol)
{
    if (symbol->base.name)
        fl_cstring_free(symbol->base.name);

    fl_free(symbol);
}
