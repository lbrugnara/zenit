#include <fllib.h>

#include "../../Test.h"
#include "../../../src/front-end/passes/parse.h"
#include "../../../src/front-end/program.h"
#include "../../../src/front-end/passes/resolve.h"
#include "../../../src/front-end/symtable.h"
#include "../../../src/front-end/types/system.h"
#include "tests.h"

void zenit_test_resolve_primitives(void)
{
    const char *source = 
        "var a : uint8 = 2;"                "\n"
        "var b = 1;"                        "\n"
        "var c : uint16 = 0x1FF;"           "\n"
        "var d = b;"                    "\n"
    ;

    struct Test {
        char *name;
        enum ZenitType decl_type;
        char *decl_type_name;
    } tests[] = {
        { "a", ZENIT_TYPE_UINT8,    "uint8"     },
        { "b", ZENIT_TYPE_UINT8,    "uint8"     },
        { "c", ZENIT_TYPE_UINT16,   "uint16"    },
        { "d", ZENIT_TYPE_UINT8,    "uint8"     },
    };

    const size_t count = sizeof(tests) / sizeof(tests[0]);

    struct ZenitContext ctx = zenit_context_new(ZENIT_SOURCE_STRING, source);
    bool is_valid = zenit_parse_source(&ctx);

    zenit_resolve_symbols(&ctx);

    for (size_t i=0; i < count; i++)
    {
        struct Test *test = tests + i;

        bool symbol_exists = zenit_program_has_symbol(ctx.program, test->name);
        fl_vexpect(symbol_exists, "Symbol table must contain symbol \"%s\"", test->name);

        struct ZenitSymbol *symbol = zenit_program_get_symbol(ctx.program, test->name);

        fl_vexpect(symbol->typeinfo->type == test->decl_type, "Symbol type must be equals to '%s'", test->decl_type_name);
    }

    zenit_context_free(&ctx);
}


void zenit_test_resolve_references(void)
{
    const char *source = 
        "var a = 2;"                    "\n"
        "var b = &a;"                   "\n"
        "var c : &uint8 = &a;"          "\n"
        "var d : uint16 = 0x1FF;"       "\n"
        "var e : &uint16 = &d;"         "\n"
    ;

    struct Test {
        char *name;
        enum ZenitType decl_type;
        char *decl_type_name;
        enum ZenitType referred_type;
        char *referred_type_name;
    } tests[] = {
        { "b", ZENIT_TYPE_REFERENCE, "&uint8",      ZENIT_TYPE_UINT8,   "uint8"     },
        { "c", ZENIT_TYPE_REFERENCE, "&uint8",      ZENIT_TYPE_UINT8,   "uint8"     },
        { "e", ZENIT_TYPE_REFERENCE, "&uint16",     ZENIT_TYPE_UINT16,  "uint16"    },
    };

    const size_t count = sizeof(tests) / sizeof(tests[0]);

    struct ZenitContext ctx = zenit_context_new(ZENIT_SOURCE_STRING, source);
    bool is_valid = zenit_parse_source(&ctx);

    zenit_resolve_symbols(&ctx);

    for (size_t i=0; i < count; i++)
    {
        struct Test *test = tests + i;

        bool symbol_exists = zenit_program_has_symbol(ctx.program, test->name);
        fl_vexpect(symbol_exists, "Symbol table must contain symbol \"%s\"", test->name);

        struct ZenitSymbol *symbol = zenit_program_get_symbol(ctx.program, test->name);

        fl_vexpect(symbol->typeinfo->type == test->decl_type, "Symbol '%s' type must be equals to '%s'", symbol->name, test->decl_type_name);

        if (test->decl_type != ZENIT_TYPE_NONE)
        {
            struct ZenitReferenceTypeInfo *ref_typeinfo = (struct ZenitReferenceTypeInfo*) symbol->typeinfo;

            fl_vexpect(ref_typeinfo->element->type == test->referred_type, "Type of the referenced symbol must be equals to '%s'", test->referred_type_name);
        }
    }

    zenit_context_free(&ctx);
}



// void zenit_test_resolve_variables(void)
// {
//     const char *source = 
//         "var a : uint8 = 2;"                "\n"
//         "var b : [2]uint8 = [ 1, 2 ];"      "\n"
//         "var c = 1;"                        "\n"
//         "var d = [ 1, 2, 3 ];"              "\n"
//         "var e : customType = 0;"           "\n"
//         "var f : [0]customType = [];"       "\n"
//         "var g = c;"                    "\n"
//         "var h : [1]uint8 = [ g ];"     "\n"
//         "var i : &uint8 = &g;"          "\n"
//         "var j = &g;"                   "\n"
//     ;

//     const char *names[] = { 
//         "a", 
//         "b",
//         "c",
//         "d",
//         "e",
//         "f",
//         "g",
//         "h",
//         "i",
//         "j",
//     };
    
//     const struct ZenitTypeInfo types[] = {
//         /* The type and number of elements is present in the variable declaration                               */
//         { .elements = 1, .name = NULL, .type = ZENIT_TYPE_UINT8, .is_array = false, .is_ref = false             },

//         /* The type and number of elements is present in the variable declaration                               */
//         { .elements = 2, .name = NULL, .type = ZENIT_TYPE_UINT8, .is_array = true, .is_ref = false              },

//         /* The type information will be inferred by the assignment, so the symbol                               */
//         /* definition does not contain that information at this pass.                                           */
//         { .elements = 0, .name = NULL, .type = ZENIT_TYPE_NONE, .is_array = false, .is_ref = false              },

//         /* Similar to the previous case, the type is not defined                                                */
//         { .elements = 0, .name = NULL, .type = ZENIT_TYPE_NONE, .is_array = false, .is_ref = false              },

//         /* The type and number of elements is present in the variable declaration                               */
//         { .elements = 1, .name = "customType", .type = ZENIT_TYPE_STRUCT, .is_array = false, .is_ref = false    },

//         /* The type and number of elements is present in the variable declaration                               */
//         { .elements = 0, .name = "customType", .type = ZENIT_TYPE_STRUCT, .is_array = true, .is_ref = false     },

//         /* The type information will be inferred by the assignment, so the symbol                               */
//         /* definition does not contain that information at this pass.                                           */
//         { .elements = 0, .name = NULL, .type = ZENIT_TYPE_NONE, .is_array = false, .is_ref = false              },

//         /* The type and number of elements is present in the variable declaration                               */
//         { .elements = 1, .name = NULL, .type = ZENIT_TYPE_UINT8, .is_array = true, .is_ref = false              },

//         /* The type information is inferred from the assignment                                                 */
//         { .elements = 1, .name = NULL, .type = ZENIT_TYPE_UINT8, .is_array = false, .is_ref = true              },

//         /* The type information will be inferred by the assignment, so the symbol                               */
//         /* definition does not contain that information at this pass.                                           */
//         { .elements = 0, .name = NULL, .type = ZENIT_TYPE_NONE, .is_array = false, .is_ref = false              },
//     };

//     const size_t count = sizeof(types) / sizeof(types[0]);

//     struct ZenitContext ctx = zenit_context_new(ZENIT_SOURCE_STRING, source);
//     bool is_valid = zenit_parse_source(&ctx);

//     zenit_resolve_symbols(&ctx);
    
//     fl_vexpect(fl_hashtable_length(ctx.program->current_scope->symtable.symbols) == count, "Symbol table must contain %zu symbols", count);

//     for (size_t i=0; i < count; i++)
//     {
//         fl_vexpect(zenit_program_has_symbol(ctx.program, names[i]), "Symbol table must contain symbol \"%s\"", names[i]);

//         struct ZenitSymbol *symbol = zenit_program_get_symbol(ctx.program, names[i]);

//         fl_vexpect(flm_cstring_equals(symbol->name, names[i]), "Symbol name must match (%s)", names[i]);
//         fl_vexpect(symbol->typeinfo.elements == types[i].elements, "Symbol elements must match (%zu)", types[i].elements);

//         if (types[i].is_array)
//             fl_expect("Symbol must be an array", symbol->typeinfo.is_array);
//         else
//             fl_expect("Symbol must not be an array", !symbol->typeinfo.is_array);

//         if (types[i].is_ref)
//             fl_expect("Symbol must be a reference", symbol->typeinfo.is_ref);
//         else
//             fl_expect("Symbol must not be a reference", !symbol->typeinfo.is_ref);

//         fl_vexpect(symbol->typeinfo.type == types[i].type, "Symbol type must match (%s)", zenit_type_to_string(types + i));
//         fl_vexpect(((symbol->typeinfo.name == NULL && types[i].name == NULL) 
//             || (flm_cstring_equals(symbol->typeinfo.name, types[i].name))), "Symbol type name must match (%s)", types[i].name ? types[i].name : "null");
//     }

//     zenit_context_free(&ctx);
// }
