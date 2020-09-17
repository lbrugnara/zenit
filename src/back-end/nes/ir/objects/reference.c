#include <fllib/Cstring.h>
#include "reference.h"
#include "../../nes.h"

ZnesReferenceAlloc* znes_reference_alloc_new(const char *name, ZnesSegmentKind segment, size_t size, uint16_t address)
{
    ZnesReferenceAlloc *ref_symbol = fl_malloc(sizeof(ZnesReferenceAlloc));
    ref_symbol->base.address = address;
    ref_symbol->base.name = name != NULL ? fl_cstring_dup(name) : NULL;
    ref_symbol->base.segment = segment;
    ref_symbol->base.type = ZNES_ALLOC_TYPE_REFERENCE;
    ref_symbol->base.size = ZNES_POINTER_SIZE;

    return ref_symbol;
}

void znes_reference_alloc_free(ZnesReferenceAlloc *symbol)
{
    if (symbol->base.name)
        fl_cstring_free(symbol->base.name);

    fl_free(symbol);
}
