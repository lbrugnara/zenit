#include <fllib.h>

#include "../../Test.h"
#include "../../../src/front-end/infer.h"
#include "../../../src/front-end/parse.h"
#include "../../../src/front-end/program.h"
#include "../../../src/front-end/resolve.h"
#include "../../../src/front-end/symtable.h"
#include "tests.h"

void zenit_test_infer_variable_type(void)
{
    const char *source = 
        "var sym_a : uint8 = 2;"                                        "\n"
        "var sym_b : [2]uint8 = [ 1, 2 ];"                              "\n"
        "var sym_c = 1;"                                                "\n"
        "var sym_d = [ 1, 2, 3 ];"                                      "\n"
        "var sym_e : customType = 0;"                                   "\n"
        "var sym_f : [0]customType = [];"                               "\n"
        "var sym_g = sym_c;"                                            "\n"
        "var sym_h : [1]uint8 = [ sym_g ];"                             "\n"
        "var sym_i : &uint8 = &sym_g;"                                  "\n"
        "var sym_j = &sym_g;"                                           "\n"
        "var sym_k = [ 8192 ];"                                         "\n"
        "var sym_l = [ 1, 8192 ];"                                      "\n"
        "var sym_m = [ 1, 8192, 3];"                                    "\n"
        "var sym_n = [ cast(&sym_a : uint16) ];"                        "\n"
        "var sym_o : [2]uint8 = [ cast(&sym_a), cast(&sym_c) ];"        "\n"
        "var sym_p = [ 0x1, 0x200];"                                    "\n"
    ;

    const char *names[] = { 
        "sym_a", 
        "sym_b",
        "sym_c",
        "sym_d",
        "sym_e",
        "sym_f",
        "sym_g",
        "sym_h",
        "sym_i",
        "sym_j",
        "sym_k",
        "sym_l",
        "sym_m",
        "sym_n",
        "sym_o",
        "sym_p",
    };
    
    const struct ZenitTypeInfo types[] = {
        /* The type and number of elements is present in the variable declaration                               */
        { .elements = 1, .name = NULL, .type = ZENIT_TYPE_UINT8, .is_array = false, .is_ref = false             },

        /* The type and number of elements is present in the variable declaration                               */
        { .elements = 2, .name = NULL, .type = ZENIT_TYPE_UINT8, .is_array = true, .is_ref = false              },

        /* The type information is inferred from the assignment                                                 */
        { .elements = 1, .name = NULL, .type = ZENIT_TYPE_UINT8, .is_array = false, .is_ref = false             },

        /* Similar to the previous case, the type is not defined                                                */
        { .elements = 3, .name = NULL, .type = ZENIT_TYPE_UINT8, .is_array = true, .is_ref = false              },

        /* Nothing to infer here, but we want to make sure this pass doesn't affect it                          */
        { .elements = 1, .name = "customType", .type = ZENIT_TYPE_CUSTOM, .is_array = false, .is_ref = false    },

        /* Nothing to infer here, but we want to make sure this pass doesn't affect it                          */
        { .elements = 0, .name = "customType", .type = ZENIT_TYPE_CUSTOM, .is_array = true, .is_ref = false     },

        /* The type information is inferred from the assignment                                                 */
        { .elements = 1, .name = NULL, .type = ZENIT_TYPE_UINT8, .is_array = false, .is_ref = false             },

        /* The type and number of elements is present in the variable declaration                               */
        { .elements = 1, .name = NULL, .type = ZENIT_TYPE_UINT8, .is_array = true, .is_ref = false              },

        /* The type information is inferred from the assignment                                                 */
        { .elements = 1, .name = NULL, .type = ZENIT_TYPE_UINT8, .is_array = false, .is_ref = true              },

        /* The type and number of elements is present in the variable declaration                               */
        { .elements = 1, .name = NULL, .type = ZENIT_TYPE_UINT8, .is_array = false, .is_ref = true              },

        /* The type information is inferred from the assignment                                                 */
        { .elements = 1, .name = NULL, .type = ZENIT_TYPE_UINT16, .is_array = true, .is_ref = false             },

        /* The type information is inferred from the assignment                                                 */
        { .elements = 2, .name = NULL, .type = ZENIT_TYPE_UINT16, .is_array = true, .is_ref = false             },

        /* The type information is inferred from the assignment                                                 */
        { .elements = 3, .name = NULL, .type = ZENIT_TYPE_UINT16, .is_array = true, .is_ref = false             },

        /* The type information is inferred from the assignment                                                 */
        { .elements = 1, .name = NULL, .type = ZENIT_TYPE_UINT16, .is_array = true, .is_ref = false             },

        /* The type information is inferred from the assignment                                                 */
        { .elements = 2, .name = NULL, .type = ZENIT_TYPE_UINT8, .is_array = true, .is_ref = false              },

        /* The type information is inferred from the assignment                                                 */
        { .elements = 2, .name = NULL, .type = ZENIT_TYPE_UINT16, .is_array = true, .is_ref = false             },
    };

    const size_t count = sizeof(types) / sizeof(types[0]);

    struct ZenitContext ctx = zenit_context_new(ZENIT_SOURCE_STRING, source);
    bool is_valid = zenit_parse_source(&ctx);

    zenit_resolve_symbols(&ctx);
    zenit_infer_types(&ctx);
    
    fl_vexpect(fl_hashtable_length(ctx.program->global_scope->symtable.symbols) == count, "Symbol table must contain %zu symbols", count);

    for (size_t i=0; i < count; i++)
    {
        fl_vexpect(zenit_program_has_global_symbol(ctx.program, names[i]), "Symbol table must contain symbol \"%s\"", names[i]);

        struct ZenitSymbol *symbol = zenit_program_get_global_symbol(ctx.program, names[i]);

        fl_vexpect(flm_cstring_equals(symbol->name, names[i]), "Symbol name must match (%s)", names[i]);
        fl_vexpect(symbol->typeinfo.elements == types[i].elements, "Symbol elements must match (%zu)", types[i].elements);
        
        if (types[i].is_array)
            fl_expect("Symbol must be an array", symbol->typeinfo.is_array);
        else
            fl_expect("Symbol must not be an array", !symbol->typeinfo.is_array);

        if (types[i].is_ref)
            fl_expect("Symbol must be a reference", symbol->typeinfo.is_ref);
        else
            fl_expect("Symbol must not be a reference", !symbol->typeinfo.is_ref);

        fl_vexpect(symbol->typeinfo.type == types[i].type, "Symbol type must match (%s)", zenit_type_to_string(types + i));
        fl_vexpect(((symbol->typeinfo.name == NULL && types[i].name == NULL) 
            || (flm_cstring_equals(symbol->typeinfo.name, types[i].name))), "Symbol type name must match (%s)", types[i].name ? types[i].name : "null");
    }

    zenit_context_free(&ctx);
}
