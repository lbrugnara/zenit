#include <fllib/Cstring.h>
#include "symbol.h"
#include "../../../zir/types/system.h"

#include "array.h"
#include "bool.h"
#include "reference.h"
#include "struct.h"
#include "uint.h"
#include "temp.h"

struct ZenitNesSymbol* zenit_nes_symbol_new(const char *name, struct ZirType *zir_type, enum ZenitNesSegment segment, uint16_t address)
{
    switch (zir_type->typekind)
    {
        case ZIR_TYPE_UINT:
            return (struct ZenitNesSymbol*) zenit_nes_symbol_uint_new(name, (struct ZirUintType*) zir_type, segment, address);

        case ZIR_TYPE_BOOL:
            return (struct ZenitNesSymbol*) zenit_nes_symbol_bool_new(name, (struct ZirBoolType*) zir_type, segment, address);

        case ZIR_TYPE_ARRAY:
            return (struct ZenitNesSymbol*) zenit_nes_symbol_array_new(name, (struct ZirArrayType*) zir_type, segment, address);

        case ZIR_TYPE_REFERENCE:
            return (struct ZenitNesSymbol*) zenit_nes_symbol_reference_new(name, (struct ZirReferenceType*) zir_type, segment, address);

        case ZIR_TYPE_STRUCT:
            return (struct ZenitNesSymbol*) zenit_nes_symbol_struct_new(name, (struct ZirStructType*) zir_type, segment, address);

        case ZIR_TYPE_NONE:
            return NULL;
    }

    return NULL;
}

void zenit_nes_symbol_free(struct ZenitNesSymbol *symbol)
{
    switch (symbol->symkind)
    {
        case ZENIT_NES_SYMBOL_UINT:
            zenit_nes_symbol_uint_free((struct ZenitNesUintSymbol*) symbol);
            break;

        case ZENIT_NES_SYMBOL_BOOL:
            zenit_nes_symbol_bool_free((struct ZenitNesBoolSymbol*) symbol);
            break;

        case ZENIT_NES_SYMBOL_ARRAY:
            zenit_nes_symbol_array_free((struct ZenitNesArraySymbol*) symbol);
            break;

        case ZENIT_NES_SYMBOL_STRUCT:
            zenit_nes_symbol_struct_free((struct ZenitNesStructSymbol*) symbol);
            break;

        case ZENIT_NES_SYMBOL_REFERENCE:
            zenit_nes_symbol_reference_free((struct ZenitNesReferenceSymbol*) symbol);
            break;

        case ZENIT_NES_SYMBOL_TEMP:
            zenit_nes_symbol_temp_free((struct ZenitNesTempSymbol*) symbol);
            break;

        default:
            if (symbol->name)
                fl_cstring_free(symbol->name);

            fl_free(symbol);
    }
}
