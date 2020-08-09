#include <fllib/Cstring.h>
#include "symbol.h"
#include "../../../zir/types/system.h"

#include "array.h"
#include "bool.h"
#include "reference.h"
#include "struct.h"
#include "uint.h"
#include "temp.h"

ZnesSymbol* zenit_nes_symbol_new(const char *name, ZirType *zir_type, ZnesSegment segment, uint16_t address)
{
    switch (zir_type->typekind)
    {
        case ZIR_TYPE_UINT:
            return (ZnesSymbol*) zenit_nes_symbol_uint_new(name, (ZirUintType*) zir_type, segment, address);

        case ZIR_TYPE_BOOL:
            return (ZnesSymbol*) zenit_nes_symbol_bool_new(name, (ZirBoolType*) zir_type, segment, address);

        case ZIR_TYPE_ARRAY:
            return (ZnesSymbol*) zenit_nes_symbol_array_new(name, (ZirArrayType*) zir_type, segment, address);

        case ZIR_TYPE_REFERENCE:
            return (ZnesSymbol*) zenit_nes_symbol_reference_new(name, (ZirReferenceType*) zir_type, segment, address);

        case ZIR_TYPE_STRUCT:
            return (ZnesSymbol*) zenit_nes_symbol_struct_new(name, (ZirStructType*) zir_type, segment, address);

        case ZIR_TYPE_NONE:
            return NULL;
    }

    return NULL;
}

void zenit_nes_symbol_free(ZnesSymbol *symbol)
{
    switch (symbol->symkind)
    {
        case ZENIT_NES_SYMBOL_UINT:
            zenit_nes_symbol_uint_free((ZnesUintSymbol*) symbol);
            break;

        case ZENIT_NES_SYMBOL_BOOL:
            zenit_nes_symbol_bool_free((ZnesBoolSymbol*) symbol);
            break;

        case ZENIT_NES_SYMBOL_ARRAY:
            zenit_nes_symbol_array_free((ZnesArraySymbol*) symbol);
            break;

        case ZENIT_NES_SYMBOL_STRUCT:
            zenit_nes_symbol_struct_free((ZnesStructSymbol*) symbol);
            break;

        case ZENIT_NES_SYMBOL_REFERENCE:
            zenit_nes_symbol_reference_free((ZnesReferenceSymbol*) symbol);
            break;

        case ZENIT_NES_SYMBOL_TEMP:
            zenit_nes_symbol_temp_free((ZnesTempSymbol*) symbol);
            break;

        default:
            if (symbol->name)
                fl_cstring_free(symbol->name);

            fl_free(symbol);
    }
}
