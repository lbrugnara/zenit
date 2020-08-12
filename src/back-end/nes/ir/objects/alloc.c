#include <fllib/Cstring.h>
#include "alloc.h"

#include "array.h"
#include "bool.h"
#include "reference.h"
#include "struct.h"
#include "uint.h"
#include "temp.h"

ZnesAlloc* znes_alloc_new(ZnesAllocType kind, const char *name, ZnesSegmentKind segment, size_t size, uint16_t address)
{
    switch (kind)
    {
        case ZNES_ALLOC_UINT:
            return (ZnesAlloc*) znes_uint_alloc_new(name, segment, size, address);

        case ZNES_ALLOC_BOOL:
            return (ZnesAlloc*) znes_bool_alloc_new(name, segment, size, address);

        case ZNES_ALLOC_ARRAY:
            return (ZnesAlloc*) znes_array_alloc_new(name, segment, size, address);

        case ZNES_ALLOC_REFERENCE:
            return (ZnesAlloc*) znes_reference_alloc_new(name, segment, size, address);

        case ZNES_ALLOC_STRUCT:
            return (ZnesAlloc*) znes_struct_alloc_new(name, segment, size, address);

        case ZNES_ALLOC_TEMP:
            return (ZnesAlloc*) znes_temp_alloc_new(name, size);

        default: break;

    }

    return NULL;
}

void znes_alloc_free(ZnesAlloc *symbol)
{
    switch (symbol->type)
    {
        case ZNES_ALLOC_UINT:
            znes_uint_alloc_free((ZnesUintAlloc*) symbol);
            break;

        case ZNES_ALLOC_BOOL:
            znes_bool_alloc_free((ZnesBoolAlloc*) symbol);
            break;

        case ZNES_ALLOC_ARRAY:
            znes_array_alloc_free((ZnesArrayAlloc*) symbol);
            break;

        case ZNES_ALLOC_STRUCT:
            znes_struct_alloc_free((ZnesStructAlloc*) symbol);
            break;

        case ZNES_ALLOC_REFERENCE:
            znes_reference_alloc_free((ZnesReferenceAlloc*) symbol);
            break;

        case ZNES_ALLOC_TEMP:
            znes_temp_alloc_free((ZnesTempAlloc*) symbol);
            break;

        default:
            if (symbol->name)
                fl_cstring_free(symbol->name);

            fl_free(symbol);
    }
}
