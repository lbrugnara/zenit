#include <fllib.h>

#include "../../Test.h"
#include "../../../src/front-end/passes/parse.h"
#include "../../../src/front-end/program.h"
#include "../../../src/front-end/passes/resolve.h"
#include "../../../src/front-end/symtable.h"
#include "../../../src/front-end/types/system.h"
#include "tests.h"

#include "symbols.h"

void zenit_test_resolve_too_many_symbols(void)
{
    struct ZenitContext ctx = zenit_context_new(ZENIT_SOURCE_FILE, "tests/front-end/resolve/symbols.zt");
    bool is_valid = zenit_parse_source(&ctx);

    zenit_resolve_symbols(&ctx);

    size_t number_of_symbols = 500;
    for (size_t i=0; i < number_of_symbols; i++)
        fl_vexpect(zenit_program_has_symbol(ctx.program, symbols[i]), "Symbol '%s' must exist", symbols[i]);

    zenit_context_free(&ctx);
}

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


void zenit_test_resolve_arrays(void)
{
    const char *source = 
        "var a : [2]uint8 = [ 1, 2 ];"      "\n"
        "var b = [ 1, 2, 3 ];"              "\n"
        "var c : [0]customType = [];"       "\n"
        "var d : uint16 = 1;"               "\n"
        "var e : [1]uint16 = [ d ];"        "\n"
    ;

    struct Test {
        char *name;
        enum ZenitType member_type;
        char *member_type_name;
        size_t length;
    } tests[] = {
        { "a", ZENIT_TYPE_UINT8,    "uint8",        2   },
        { "b", ZENIT_TYPE_NONE,     NULL,           3   },
        { "c", ZENIT_TYPE_STRUCT,   "customType",   0   },
        { "e", ZENIT_TYPE_UINT16,   "uint16",       1   },
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

        fl_vexpect(symbol->typeinfo->type == ZENIT_TYPE_ARRAY, "Symbol '%s' type must be equals to '%s'", symbol->name, zenit_type_to_string(symbol->typeinfo));

        if (test->member_type != ZENIT_TYPE_NONE)
        {
            struct ZenitArrayTypeInfo *array_typeinfo = (struct ZenitArrayTypeInfo*) symbol->typeinfo;

            fl_vexpect(array_typeinfo->length == test->length, "Number of elements in the array must be equals to %zu", test->length);
            fl_vexpect(array_typeinfo->member_type->type == test->member_type, "Type of the array's member type must be equals to '%s'", test->member_type_name);
        }
    }

    zenit_context_free(&ctx);
}
