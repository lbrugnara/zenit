#include <fllib/Cstring.h>
#include "symbol.h"
#include "../../../zir/types/system.h"

#include "array.h"
#include "bool.h"
#include "reference.h"
#include "struct.h"
#include "uint.h"
#include "temp.h"

ZnesSymbol* znes_symbol_new(const char *name, ZirType *zir_type, ZnesSegment segment, uint16_t address)
{
    switch (zir_type->typekind)
    {
        case ZIR_TYPE_UINT:
            return (ZnesSymbol*) znes_uint_symbol_new(name, (ZirUintType*) zir_type, segment, address);

        case ZIR_TYPE_BOOL:
            return (ZnesSymbol*) znes_bool_symbol_new(name, (ZirBoolType*) zir_type, segment, address);

        case ZIR_TYPE_ARRAY:
            return (ZnesSymbol*) znes_array_symbol_new(name, (ZirArrayType*) zir_type, segment, address);

        case ZIR_TYPE_REFERENCE:
            return (ZnesSymbol*) znes_reference_symbol_new(name, (ZirReferenceType*) zir_type, segment, address);

        case ZIR_TYPE_STRUCT:
            return (ZnesSymbol*) znes_struct_symbol_new(name, (ZirStructType*) zir_type, segment, address);

        case ZIR_TYPE_NONE:
            return NULL;
    }

    return NULL;
}

void znes_symbol_free(ZnesSymbol *symbol)
{
    switch (symbol->symkind)
    {
        case ZNES_SYMBOL_UINT:
            znes_uint_symbol_free((ZnesUintSymbol*) symbol);
            break;

        case ZNES_SYMBOL_BOOL:
            znes_bool_symbol_free((ZnesBoolSymbol*) symbol);
            break;

        case ZNES_SYMBOL_ARRAY:
            znes_array_symbol_free((ZnesArraySymbol*) symbol);
            break;

        case ZNES_SYMBOL_STRUCT:
            znes_struct_symbol_free((ZnesStructSymbol*) symbol);
            break;

        case ZNES_SYMBOL_REFERENCE:
            znes_reference_symbol_free((ZnesReferenceSymbol*) symbol);
            break;

        case ZNES_SYMBOL_TEMP:
            znes_temp_symbol_free((ZnesTempSymbol*) symbol);
            break;

        default:
            if (symbol->name)
                fl_cstring_free(symbol->name);

            fl_free(symbol);
    }
}
