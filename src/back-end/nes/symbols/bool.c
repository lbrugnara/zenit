#include <fllib/Cstring.h>
#include "bool.h"
#include "../../../zir/types/bool.h"

struct ZenitNesBoolSymbol* zenit_nes_symbol_bool_new(const char *name, struct ZirBoolType *zir_bool_type, enum ZenitNesSegment segment, uint16_t address)
{
    struct ZenitNesBoolSymbol *bool_symbol = fl_malloc(sizeof(struct ZenitNesBoolSymbol));
    bool_symbol->base.address = address;
    bool_symbol->base.name = name != NULL ? fl_cstring_dup(name) : NULL;
    bool_symbol->base.segment = segment;
    bool_symbol->base.symkind = ZENIT_NES_SYMBOL_BOOL;
    bool_symbol->base.size = zir_type_bool_size(zir_bool_type);
    
    return bool_symbol;
}

void zenit_nes_symbol_bool_free(struct ZenitNesBoolSymbol *symbol)
{
    if (symbol->base.name)
        fl_cstring_free(symbol->base.name);

    fl_free(symbol);
}
