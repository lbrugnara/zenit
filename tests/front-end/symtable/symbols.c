

#include <flut/flut.h>
#include "../../../src/front-end/symtable.h"
#include "tests.h"

void zenit_test_symtable_api(void)
{
    // struct ZenitSymtable table = zenit_symtable_new(ZENIT_SCOPE_GLOBAL, "global");

    // const char *names[] = { 
    //     "sym_a", 
    //     "sym_b",
    //     "sym_c"
    // };
    
    // const struct ZenitTypeInfo types[] = {
    //     { .elements = 1, .name = NULL, .type = ZENIT_TYPE_UINT8, .is_array = false      },
    //     { .elements = 2, .name = NULL, .type = ZENIT_TYPE_UINT8, .is_array = true       },
    //     { .elements = 3, .name = "custom", .type = ZENIT_TYPE_STRUCT, .is_array = true  }
    // };

    // const size_t count = sizeof(types) / sizeof(types[0]);

    // for (size_t i=0; i < count; i++)
    // {
    //     ZenitSymbol *symbol = zenit_symbol_new(names[i], (struct ZenitTypeInfo*)types + i);
    //     zenit_symtable_add(&table, symbol);
    // }
    
    // flut_vexpect_compat(fl_hashtable_length(table.symbols) == count, "Symbol table must contain %zu symbols", count);

    // for (size_t i=0; i < count; i++)
    // {
    //     flut_vexpect_compat(zenit_symtable_has(&table, names[i]), "Symbol table must contain symbol \"%s\"", names[i]);

    //     ZenitSymbol *symbol = zenit_symtable_get(&table, names[i]);

    //     flut_vexpect_compat(flm_cstring_equals(symbol->name, names[i]), "Symbol name must match (%s)", names[i]);
    //     flut_vexpect_compat(symbol->type.elements == types[i].elements, "Symbol elements must match (%zu)", types[i].elements);
    //     if (types[i].is_array)
    //         flut_expect_compat("Symbol must be an array", symbol->type.is_array);
    //     else
    //         flut_expect_compat("Symbol must not be an array", !symbol->type.is_array);
    //     flut_vexpect_compat(symbol->type == types[i].type, "Symbol type must match (%s)", zenit_type_to_string(types + i));
    //     flut_vexpect_compat(((symbol->type.name == NULL && types[i].name == NULL) 
    //         || (flm_cstring_equals(symbol->type.name, types[i].name))), "Symbol type name must match (%s)", types[i].name ? types[i].name : "null");
    // }

    // zenit_symtable_free(&table);
}
