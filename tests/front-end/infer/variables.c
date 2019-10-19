#include <fllib.h>

#include "../../Test.h"
#include "../../../src/front-end/infer.h"
#include "../../../src/front-end/parse.h"
#include "../../../src/front-end/resolve.h"
#include "../../../src/front-end/symtable.h"
#include "tests.h"

void cenit_test_infer_variable_type(void)
{
    const char *source = 
        "var sym_a : uint8 = 2;"                "\n"
        "var sym_b : [2]uint8 = [ 1, 2 ];"      "\n"
        "var sym_c = 1;"                        "\n"
        "var sym_d = [ 1, 2, 3 ];"              "\n"
    ;

    const char *names[] = { 
        "sym_a", 
        "sym_b",
        "sym_c",
        "sym_d",
    };
    
    const CenitTypeInfo types[] = {
        /* The type and number of elements is present in the variable declaration       */
        { .elements = 1, .name = NULL, .type = CENIT_TYPE_UINT8, .is_array = false      },

        /* The type and number of elements is present in the variable declaration       */
        { .elements = 2, .name = NULL, .type = CENIT_TYPE_UINT8, .is_array = true       },

        /* The type information will be inferred by the assignment, so the symbol       */
        /* definition does not contain that information at this pass.                   */
        { .elements = 1, .name = NULL, .type = CENIT_TYPE_UINT8, .is_array = false       },

        /* Similar to the previous case, the type is not defined                        */
        { .elements = 3, .name = NULL, .type = CENIT_TYPE_UINT8, .is_array = true        },
    };

    const size_t count = sizeof(types) / sizeof(types[0]);

    CenitContext ctx = cenit_context_new("global");
    bool is_valid = cenit_parse_string(&ctx, source);

    cenit_resolve_symbols(&ctx);
    cenit_infer_types(&ctx);
    
    fl_vexpect(fl_hashtable_length(ctx.symtable.symbols) == count, "Symbol table must contain %zu symbols", count);

    for (size_t i=0; i < count; i++)
    {
        fl_vexpect(cenit_symtable_has(&ctx.symtable, names[i]), "Symbol table must contain symbol \"%s\"", names[i]);

        CenitSymbol *symbol = cenit_symtable_get(&ctx.symtable, names[i]);

        fl_vexpect(flm_cstring_equals(symbol->name, names[i]), "Symbol name must match (%s)", names[i]);
        fl_vexpect(symbol->typeinfo.elements == types[i].elements, "Symbol elements must match (%zu)", types[i].elements);
        if (types[i].is_array)
            fl_expect("Symbol must be an array", symbol->typeinfo.is_array);
        else
            fl_expect("Symbol must not be an array", !symbol->typeinfo.is_array);
        fl_vexpect(symbol->typeinfo.type == types[i].type, "Symbol type must match (%s)", cenit_type_string(types[i].type));
        fl_vexpect(((symbol->typeinfo.name == NULL && types[i].name == NULL) 
            || (flm_cstring_equals(symbol->typeinfo.name, types[i].name))), "Symbol type name must match (%s)", types[i].name ? types[i].name : "null");
    }

    cenit_context_free(&ctx);
}
