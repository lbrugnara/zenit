#include <fllib.h>

#include "../../Test.h"
#include "../../../src/front-end/passes/infer.h"
#include "../../../src/front-end/passes/parse.h"
#include "../../../src/front-end/program.h"
#include "../../../src/front-end/types/system.h"
#include "../../../src/front-end/passes/resolve.h"
#include "../../../src/front-end/symtable.h"
#include "tests.h"

void zenit_test_infer_variable_uint(void)
{
    const char *source = 
        "var sym_a = 2;"                        "\n"
        "var sym_b = 1;"                        "\n"
        "var sym_c = sym_b;"                    "\n"
        "var sym_d = 0x1FF;"                    "\n"
        "var sym_e = cast(sym_d : uint8);"      "\n"
        "var sym_f = cast(&sym_d : uint8);"     "\n"
    ;

    const char *tests[][2] = { 
        { "sym_a", "uint8" },
        { "sym_b", "uint8" },
        { "sym_c", "uint8" },
        { "sym_d", "uint16" },
        { "sym_e", "uint8" },
        { "sym_f", "uint8" },
    };

    const size_t count = sizeof(tests) / sizeof(tests[0]);

    struct ZenitContext ctx = zenit_context_new(ZENIT_SOURCE_STRING, source);
    bool is_parsing_valid = zenit_parse_source(&ctx);
    bool is_resolve_valid = is_parsing_valid && zenit_resolve_symbols(&ctx);
    bool is_inference_valid = is_resolve_valid && zenit_infer_types(&ctx);
    
    fl_expect("Parsing, symbol resolving, and type inference passes should not contain errors", is_inference_valid);

    for (size_t i=0; i < count; i++)
    {
        const char **test = tests[i];

        fl_vexpect(zenit_program_has_symbol(ctx.program, test[0]), "Symbol table must contain symbol \"%s\"", test[0]);

        struct ZenitSymbol *symbol = zenit_program_get_symbol(ctx.program, test[0]);

        fl_vexpect(flm_cstring_equals(zenit_type_to_string(symbol->typeinfo), test[1]), "Symbol %s's type must be %s", test[0], test[1]);
    }

    zenit_context_free(&ctx);
}

void zenit_test_infer_variable_reference(void)
{
    const char *source = 
        "var sym_a = 1;"                        "\n"
        "var sym_b = 0x1FF;"                    "\n"

        "var sym_c = &sym_a;"                   "\n"
        "var sym_d = &sym_b;"                   "\n"
        "var sym_e = &sym_d;"                   "\n"
        "var sym_f = cast(&sym_b : &uint8);"    "\n"
        "var sym_g = cast(&sym_a : &uint16);"   "\n" // Not valid (it doesn't pass the type check, but the inference for sym_g should still work)
    ;
    
    const char *tests[][2] = { 
        { "sym_a", "uint8" },
        { "sym_b", "uint16" },
        { "sym_c", "&uint8" },
        { "sym_d", "&uint16" },
        { "sym_e", "&&uint16" },
        { "sym_f", "&uint8" },
        { "sym_g", "&uint16" },
    };

    const size_t count = sizeof(tests) / sizeof(tests[0]);

    struct ZenitContext ctx = zenit_context_new(ZENIT_SOURCE_STRING, source);
    bool is_parsing_valid = zenit_parse_source(&ctx);
    bool is_resolve_valid = is_parsing_valid && zenit_resolve_symbols(&ctx);
    bool is_inference_valid = is_resolve_valid && zenit_infer_types(&ctx);
    
    fl_expect("Parsing, symbol resolving, and type inference passes should not contain errors", is_inference_valid);

    for (size_t i=0; i < count; i++)
    {
        const char **test = tests[i];

        fl_vexpect(zenit_program_has_symbol(ctx.program, test[0]), "Symbol table must contain symbol \"%s\"", test[0]);

        struct ZenitSymbol *symbol = zenit_program_get_symbol(ctx.program, test[0]);

        fl_vexpect(flm_cstring_equals(zenit_type_to_string(symbol->typeinfo), test[1]), "Symbol %s's type must be %s", test[0], test[1]);
    }

    zenit_context_free(&ctx);
}

void zenit_test_infer_variable_array(void)
{
    const char *source = 
        "var sym_a : uint8 = 2;"                                        "\n"
        "var sym_b : uint16 = 1;"                                       "\n"
        "var sym_c : customType = 0;"                                   "\n"
        "var sym_d = sym_c;"                                            "\n"
        "var sym_e : &uint16 = &sym_b;"                                  "\n"
        "var sym_f = &sym_e;"                                           "\n"
        "var sym_g : [2]uint8 = [ 1, 2 ];"                              "\n"
        "var sym_h = [ 1, 2, 3 ];"                                      "\n"
        "var sym_i : [0]customType = [];"                               "\n"
        "var sym_j : [1]uint16 = [ sym_b ];"                            "\n"
        "var sym_k = [ 8192 ];"                                         "\n"
        "var sym_l = [ 1, 8192 ];"                                      "\n"
        "var sym_m = [ 1, 8192, 3];"                                    "\n"
        "var sym_n = [ cast(&sym_a : uint16) ];"                        "\n"
        "var sym_o : [2]uint8 = [ cast(&sym_a), cast(&sym_b) ];"        "\n"
        "var sym_p = [ 0x1, 0x200];"                                    "\n"
        "var sym_q : [2]&uint8 = [ &sym_a, cast(&sym_b : &uint8) ];"    "\n"
        "var sym_r : [2]uint16 = [ 0x1, 0x2 ];"                         "\n"
        "var sym_s : [2]uint8 = [ 0x1FF, 0x2FF ];"                      "\n"
        "var sym_t = [ sym_r, sym_s ];"                                 "\n"
        "var sym_u = [ cast(sym_r : [2]uint8), sym_s ];"                "\n"
        "var sym_v = [ &sym_r, &sym_s ];"                               "\n"
        "var sym_w = [ cast(&sym_r : &[2]uint8), &sym_s ];"             "\n"
        "var sym_x = [ sym_w ];"                                        "\n"
        "var sym_y : [][][]&[]uint8 = [ sym_x ];"                       "\n"
        "var sym_z : []uint8 = cast(sym_m : []uint8);"                  "\n"
    ;

    const char *tests[][2] = { 
        {   "sym_g",  "[2]uint8",           },
        {   "sym_h",  "[3]uint8",           },
        {   "sym_i",  "[0]customType",      },
        {   "sym_j",  "[1]uint16",          },
        {   "sym_k",  "[1]uint16",          },
        {   "sym_l",  "[2]uint16",          },
        {   "sym_m",  "[3]uint16",          },
        {   "sym_n",  "[1]uint16",          },
        {   "sym_o",  "[2]uint8",           },
        {   "sym_p",  "[2]uint16",          },
        {   "sym_q",  "[2]&uint8",          },
        {   "sym_t",  "[2][2]uint16",       },
        {   "sym_u",  "[2][2]uint8",        },
        {   "sym_v",  "[2]&[2]uint16",      },
        {   "sym_w",  "[2]&[2]uint8",       },
        {   "sym_x",  "[1][2]&[2]uint8",    },
        {   "sym_y",  "[1][1][2]&[2]uint8", },
        {   "sym_z",  "[3]uint8",           },
    };

    const size_t count = sizeof(tests) / sizeof(tests[0]);

    struct ZenitContext ctx = zenit_context_new(ZENIT_SOURCE_STRING, source);
    bool is_parsing_valid = zenit_parse_source(&ctx);
    bool is_resolve_valid = is_parsing_valid && zenit_resolve_symbols(&ctx);
    bool is_inference_valid = is_resolve_valid && zenit_infer_types(&ctx);
    
    fl_expect("Parsing, symbol resolving, and type inference passes should not contain errors", is_inference_valid);

    for (size_t i=0; i < count; i++)
    {
        const char **test = tests[i];

        fl_vexpect(zenit_program_has_symbol(ctx.program, test[0]), "Symbol table must contain symbol \"%s\"", test[0]);

        struct ZenitSymbol *symbol = zenit_program_get_symbol(ctx.program, test[0]);

        fl_vexpect(flm_cstring_equals(zenit_type_to_string(symbol->typeinfo), test[1]), "Symbol %s's type must be %s", test[0], test[1]);
    }

    zenit_context_free(&ctx);
}
