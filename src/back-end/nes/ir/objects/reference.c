#include <fllib/Cstring.h>
#include "reference.h"

ZnesReferenceAlloc* znes_reference_alloc_new(const char *name, ZnesSegmentKind segment, size_t size, uint16_t address)
{
    ZnesReferenceAlloc *ref_symbol = fl_malloc(sizeof(ZnesReferenceAlloc));
    ref_symbol->base.address = address;
    ref_symbol->base.name = name != NULL ? fl_cstring_dup(name) : NULL;
    ref_symbol->base.segment = segment;
    ref_symbol->base.type = ZNES_ALLOC_REFERENCE;
    ref_symbol->base.size = 2 /* bytes */;

    return ref_symbol;
}

void znes_reference_alloc_free(ZnesReferenceAlloc *symbol)
{
    if (symbol->base.name)
        fl_cstring_free(symbol->base.name);

    fl_free(symbol);
}
