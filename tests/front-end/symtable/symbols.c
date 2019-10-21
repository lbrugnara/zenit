#include <fllib.h>

#include "../../Test.h"
#include "../../../src/front-end/symtable.h"
#include "tests.h"

void cenit_test_symtable_api(void)
{
    CenitSymbolTable table = cenit_symtable_new(CENIT_SYMTABLE_GLOBAL, "global");

    const char *names[] = { 
        "sym_a", 
        "sym_b",
        "sym_c"
    };
    
    const CenitTypeInfo types[] = {
        { .elements = 1, .name = NULL, .type = CENIT_TYPE_UINT8, .is_array = false      },
        { .elements = 2, .name = NULL, .type = CENIT_TYPE_UINT8, .is_array = true       },
        { .elements = 3, .name = "custom", .type = CENIT_TYPE_CUSTOM, .is_array = true  }
    };

    const size_t count = sizeof(types) / sizeof(types[0]);

    for (size_t i=0; i < count; i++)
    {
        CenitSymbol *symbol = cenit_symbol_new(names[i], (CenitTypeInfo*)types + i);
        cenit_symtable_add(&table, symbol);
    }
    
    fl_vexpect(fl_hashtable_length(table.symbols) == count, "Symbol table must contain %zu symbols", count);

    for (size_t i=0; i < count; i++)
    {
        fl_vexpect(cenit_symtable_has(&table, names[i]), "Symbol table must contain symbol \"%s\"", names[i]);

        CenitSymbol *symbol = cenit_symtable_get(&table, names[i]);

        fl_vexpect(flm_cstring_equals(symbol->name, names[i]), "Symbol name must match (%s)", names[i]);
        fl_vexpect(symbol->typeinfo.elements == types[i].elements, "Symbol elements must match (%zu)", types[i].elements);
        if (types[i].is_array)
            fl_expect("Symbol must be an array", symbol->typeinfo.is_array);
        else
            fl_expect("Symbol must not be an array", !symbol->typeinfo.is_array);
        fl_vexpect(symbol->typeinfo.type == types[i].type, "Symbol type must match (%s)", cenit_type_to_string(types + i));
        fl_vexpect(((symbol->typeinfo.name == NULL && types[i].name == NULL) 
            || (flm_cstring_equals(symbol->typeinfo.name, types[i].name))), "Symbol type name must match (%s)", types[i].name ? types[i].name : "null");
    }

    cenit_symtable_free(&table);
}
