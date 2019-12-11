#include <fllib.h>

#include "../../Test.h"
#include "../../../src/front-end/passes/infer.h"
#include "../../../src/front-end/passes/parse.h"
#include "../../../src/front-end/program.h"
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
    ;
    
    struct TestCase {
        char *name;
        enum ZenitType type;
        enum ZenitUintTypeSize size;
        char *type_name;
    } tests[] = {
        { "sym_a", ZENIT_TYPE_UINT, ZENIT_UINT_8,    "uint8"  },
        { "sym_b", ZENIT_TYPE_UINT, ZENIT_UINT_8,    "uint8"  },
        { "sym_c", ZENIT_TYPE_UINT, ZENIT_UINT_8,    "uint8"  },
        { "sym_d", ZENIT_TYPE_UINT, ZENIT_UINT_16,   "uint16" },
        { "sym_e", ZENIT_TYPE_UINT, ZENIT_UINT_8,    "uint8"  },
    };

    const size_t count = sizeof(tests) / sizeof(tests[0]);

    struct ZenitContext ctx = zenit_context_new(ZENIT_SOURCE_STRING, source);
    bool is_parsing_valid = zenit_parse_source(&ctx);
    bool is_resolve_valid = is_parsing_valid && zenit_resolve_symbols(&ctx);
    bool is_inference_valid = is_resolve_valid && zenit_infer_types(&ctx);
    
    fl_expect("Parsing, symbol resolving, and type inference passes should not contain errors", is_inference_valid);

    for (size_t i=0; i < count; i++)
    {
        struct TestCase *test = tests + i;

        fl_vexpect(zenit_program_has_symbol(ctx.program, test->name), "Symbol table must contain symbol \"%s\"", test->name);

        struct ZenitSymbol *symbol = zenit_program_get_symbol(ctx.program, test->name);

        fl_vexpect(symbol->typeinfo->type == test->type
            && ((struct ZenitUintTypeInfo*) symbol->typeinfo)->size == test->size,
            "Symbol %s's type must be %s", test->name, test->type_name);
    }

    zenit_context_free(&ctx);
}


void zenit_test_infer_variable_reference(void)
{
    const char *source = 
        "var sym_a = 1;"                                                "\n"
        "var sym_b = 0x1FF;"                                            "\n"

        "var sym_c = &sym_a;"                                           "\n"
        "var sym_d = &sym_b;"                                           "\n"
        "var sym_e = &sym_d;"                                           "\n"
    ;
    
    struct TestCase {
        char *name;
        enum ZenitType ref_type;
        enum ZenitUintTypeSize ref_size;
        char *ref_type_name;
    } tests[] = {
        { "sym_c", ZENIT_TYPE_UINT, ZENIT_UINT_8,        "uint8"     },
        { "sym_d", ZENIT_TYPE_UINT, ZENIT_UINT_16,       "uint16"    },
        { "sym_e", ZENIT_TYPE_UINT, ZENIT_UINT_16,       "uint16"   },
    };

    const size_t count = sizeof(tests) / sizeof(tests[0]);

    struct ZenitContext ctx = zenit_context_new(ZENIT_SOURCE_STRING, source);
    bool is_parsing_valid = zenit_parse_source(&ctx);
    bool is_resolve_valid = is_parsing_valid && zenit_resolve_symbols(&ctx);
    bool is_inference_valid = is_resolve_valid && zenit_infer_types(&ctx);
    
    fl_expect("Parsing, symbol resolving, and type inference passes should not contain errors", is_inference_valid);

    for (size_t i=0; i < count; i++)
    {
        struct TestCase *test = tests + i;

        fl_vexpect(zenit_program_has_symbol(ctx.program, test->name), "Symbol table must contain symbol \"%s\"", test->name);

        struct ZenitSymbol *symbol = zenit_program_get_symbol(ctx.program, test->name);

        struct ZenitReferenceTypeInfo *ref_typeinfo = (struct ZenitReferenceTypeInfo*) symbol->typeinfo;

        if (ref_typeinfo->element->type == ZENIT_TYPE_UINT)
        {
            fl_vexpect(ref_typeinfo->element->type == test->ref_type
                && ((struct ZenitUintTypeInfo*) ref_typeinfo->element)->size == test->ref_size,
                "Underlying type of the referenced variable in symbol %s's type must be %s", test->name, test->ref_type_name);
        }
        else if (ref_typeinfo->element->type == ZENIT_TYPE_REFERENCE)
        {
            struct ZenitReferenceTypeInfo *ref_elem_typeinfo = (struct ZenitReferenceTypeInfo*) ref_typeinfo->element;
            fl_vexpect(ref_elem_typeinfo->element->type == test->ref_type, "Underlying type of the referenced variable in symbol %s's type must be %s", test->name, test->ref_type_name);
        }
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
    ;

    struct TestCase {
        char *name;
        enum ZenitType member_type;
        enum ZenitUintTypeSize member_type_size;
        char *member_type_name;
        size_t length;
        enum ZenitType ref_member_type;
        enum ZenitUintTypeSize ref_member_type_size;
    } tests[] = {
        {   "sym_g",  ZENIT_TYPE_UINT,          ZENIT_UINT_8,       "uint8",        2,  ZENIT_TYPE_NONE,    ZENIT_UINT_UNK     },
        {   "sym_h",  ZENIT_TYPE_UINT,          ZENIT_UINT_8,       "uint8",        3,  ZENIT_TYPE_NONE,    ZENIT_UINT_UNK     },
        {   "sym_i",  ZENIT_TYPE_STRUCT,        ZENIT_UINT_UNK,     "customType",   0,  ZENIT_TYPE_NONE,    ZENIT_UINT_UNK     },
        {   "sym_j",  ZENIT_TYPE_UINT,          ZENIT_UINT_16,      "uint16",       1,  ZENIT_TYPE_NONE,    ZENIT_UINT_UNK     },
        {   "sym_k",  ZENIT_TYPE_UINT,          ZENIT_UINT_16,      "uint16",       1,  ZENIT_TYPE_NONE,    ZENIT_UINT_UNK     },
        {   "sym_l",  ZENIT_TYPE_UINT,          ZENIT_UINT_16,      "uint16",       2,  ZENIT_TYPE_NONE,    ZENIT_UINT_UNK     },
        {   "sym_m",  ZENIT_TYPE_UINT,          ZENIT_UINT_16,      "uint16",       3,  ZENIT_TYPE_NONE,    ZENIT_UINT_UNK     },
        {   "sym_n",  ZENIT_TYPE_UINT,          ZENIT_UINT_16,      "uint16",       1,  ZENIT_TYPE_NONE,    ZENIT_UINT_UNK     },
        {   "sym_o",  ZENIT_TYPE_UINT,          ZENIT_UINT_8,       "uint8",        2,  ZENIT_TYPE_NONE,    ZENIT_UINT_UNK     },
        {   "sym_p",  ZENIT_TYPE_UINT,          ZENIT_UINT_16,      "uint16",       2,  ZENIT_TYPE_NONE,    ZENIT_UINT_UNK     },
        {   "sym_q",  ZENIT_TYPE_REFERENCE,     ZENIT_UINT_UNK,     "&uint8",       2,  ZENIT_TYPE_UINT,    ZENIT_UINT_8       },
    };

    const size_t count = sizeof(tests) / sizeof(tests[0]);

    struct ZenitContext ctx = zenit_context_new(ZENIT_SOURCE_STRING, source);
    bool is_parsing_valid = zenit_parse_source(&ctx);
    bool is_resolve_valid = is_parsing_valid && zenit_resolve_symbols(&ctx);
    bool is_inference_valid = is_resolve_valid && zenit_infer_types(&ctx);
    
    fl_expect("Parsing, symbol resolving, and type inference passes should not contain errors", is_inference_valid);

    for (size_t i=0; i < count; i++)
    {
        struct TestCase *test = tests + i;

        fl_vexpect(zenit_program_has_symbol(ctx.program, test->name), "Symbol table must contain symbol \"%s\"", test->name);

        struct ZenitSymbol *symbol = zenit_program_get_symbol(ctx.program, test->name);

        fl_vexpect(symbol->typeinfo->type == ZENIT_TYPE_ARRAY, "Symbol '%s' must be an array", symbol->name);

        struct ZenitArrayTypeInfo *array_typeinfo = (struct ZenitArrayTypeInfo*) symbol->typeinfo;

        fl_vexpect(array_typeinfo->length == test->length, "Symbol '%s' must be an array of %zu elements", symbol->name, test->length);

        if (test->ref_member_type == ZENIT_TYPE_NONE)
        {
            fl_vexpect(array_typeinfo->member_type->type == test->member_type
                && (test->member_type != ZENIT_TYPE_UINT || ((struct ZenitUintTypeInfo*)array_typeinfo->member_type)->size == test->member_type_size),
                "Symbol '%s' must be an array of '%s'", symbol->name, test->member_type_name);
        } 
        else if (test->ref_member_type != ZENIT_TYPE_NONE)
        {
            bool is_ref = array_typeinfo->member_type->type == test->member_type;
            bool ref_type_is_valid = array_typeinfo->member_type->type == ZENIT_TYPE_REFERENCE
                                    && ((struct ZenitReferenceTypeInfo*) array_typeinfo->member_type)->element->type == test->ref_member_type;

            bool type_size_is_valid = true;

            if (is_ref)
            {
                struct ZenitTypeInfo *ref_element = ((struct ZenitReferenceTypeInfo*) array_typeinfo->member_type)->element;
                if (ref_element->type == ZENIT_TYPE_UINT)
                    type_size_is_valid = ((struct ZenitUintTypeInfo*) ref_element)->size == test->ref_member_type_size;
            }


            fl_vexpect(is_ref && ref_type_is_valid && type_size_is_valid, "Symbol '%s' must be an array of '%s'", symbol->name, test->member_type_name);
        }
    }

    zenit_context_free(&ctx);
}
