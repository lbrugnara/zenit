#include <fllib.h>

#include "../../Test.h"
#include "../../../src/front-end/ast/ast.h"
#include "../../../src/front-end/context.h"
#include "../../../src/front-end/lexer.h"
#include "../../../src/front-end/passes/parse.h"
#include "tests.h"

void zenit_test_parser_literal_integer(void)
{
    const char *source = 
            "/* uint8 */ 1;"                    "\n"
            "/* uint8 */ 2;"                    "\n"
            "/* uint8 */ 3;"                    "\n"
            "/* uint8 */ 255;"                  "\n"
            "/* uint16 */ 8192;"                "\n"
            //"/* uint64 */ 1844674407370955161;  \n"
    ;

    const size_t results[] = { 1, 2, 3, 255, 8192 };
    const enum ZenitType types[] = { ZENIT_TYPE_UINT8, ZENIT_TYPE_UINT8, ZENIT_TYPE_UINT8, ZENIT_TYPE_UINT8, ZENIT_TYPE_UINT16 };

    struct ZenitContext ctx = zenit_context_new(ZENIT_SOURCE_STRING, source);

    bool is_valid = zenit_parse_source(&ctx);

    fl_expect("Parser object should not contain errors", is_valid);

    for (size_t i=0; i < fl_array_length(ctx.ast->decls); i++)
    {
        struct ZenitNode *node = ctx.ast->decls[i];
        fl_expect("Node must be a literal node", node->type == ZENIT_NODE_LITERAL);

        struct ZenitPrimitiveNode *literal = (struct ZenitPrimitiveNode*)node;
        fl_vexpect(literal->base.typeinfo.type == types[i], "Literal type must be \"%s\"", zenit_type_to_string(&literal->base.typeinfo));

        switch (types[i])
        {
            case ZENIT_TYPE_UINT8:
                fl_vexpect((size_t)literal->value.uint8 == results[i], "Literal value must be equals to %zu", results[i]);
                break;
            case ZENIT_TYPE_UINT16:
                fl_vexpect((size_t)literal->value.uint16 == results[i], "Literal value must be equals to %zu", results[i]);
                break;
            default:
                fl_expect("Unmanaged type", false);
        }        
    }    

    zenit_context_free(&ctx);
}

void zenit_test_parser_literal_array_initializer(void)
{
    const char *source = 
            /* [3]uint8 */
            "[ 1, 2, 3 ];"      "\n"
            "[ ];"              "\n"
    ;

    const size_t elements[] = { 3, 0 };
    const size_t values[99][99] = { { 1, 2, 3 }, { 0 } };

    struct ZenitContext ctx = zenit_context_new(ZENIT_SOURCE_STRING, source);
    bool is_valid = zenit_parse_source(&ctx);

    fl_expect("Parser object should not contain errors", is_valid);

    for (size_t i=0; i < fl_array_length(ctx.ast->decls); i++)
    {
        struct ZenitNode *node = ctx.ast->decls[i];
        fl_expect("Node must be a literal node", node->type == ZENIT_NODE_ARRAY);

        struct ZenitArrayNode *array = (struct ZenitArrayNode*)node;

        fl_vexpect(array->elements && fl_array_length(array->elements) == elements[i], "Array must contain %zu elements", elements[i]);

        for (size_t j=0; j < fl_array_length(array->elements); j++)
        {
            struct ZenitPrimitiveNode *literal = (struct ZenitPrimitiveNode*)array->elements[j];
            fl_vexpect(literal->base.typeinfo.type == ZENIT_TYPE_UINT8, "Literal type must be \"%s\"", zenit_type_to_string(&literal->base.typeinfo));
            fl_vexpect((size_t)literal->value.uint8 == values[i][j], "Literal value must be equals to %zu", values[i][j]);
        }
    }    

    zenit_context_free(&ctx);
}

void zenit_test_parser_literal_integer_error(void)
{
    const char *source = 
        /* ULLONG_MAX + 1: Too large integral type*/
        "18446744073709551616;          \n"
    ;

    struct ZenitContext ctx = zenit_context_new(ZENIT_SOURCE_STRING, source);
    bool is_valid = zenit_parse_source(&ctx);

    size_t expected_errors = 1;
    fl_vexpect(!is_valid && ctx.errors != NULL && fl_array_length(ctx.errors) == expected_errors, "The context object must contain %zu error(s)", expected_errors);

    struct ZenitError *error = ctx.errors;

    fl_vexpect(error->location.line == 1 && error->type == ZENIT_ERROR_LARGE_INTEGER, 
        "Expected semantic error: %s at line %u:%u", error->message, error->location.line, error->location.col);

    zenit_context_free(&ctx);
}
