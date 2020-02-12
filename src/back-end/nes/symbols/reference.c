#include <fllib/Cstring.h>
#include "reference.h"
#include "../../../zir/types/reference.h"

struct ZenitNesReferenceSymbol* zenit_nes_symbol_reference_new(const char *name, struct ZirReferenceType *zir_reference_type, enum ZenitNesSegment segment, uint16_t address)
{
    struct ZenitNesReferenceSymbol *ref_symbol = fl_malloc(sizeof(struct ZenitNesReferenceSymbol));
    ref_symbol->base.address = address;
    ref_symbol->base.name = name != NULL ? fl_cstring_dup(name) : NULL;
    ref_symbol->base.segment = segment;
    ref_symbol->base.symkind = ZENIT_NES_SYMBOL_REFERENCE;
    ref_symbol->base.size = zir_type_reference_size(zir_reference_type);

    return ref_symbol;
}

void zenit_nes_symbol_reference_free(struct ZenitNesReferenceSymbol *symbol)
{
    if (symbol->base.name)
        fl_cstring_free(symbol->base.name);

    fl_free(symbol);
}
