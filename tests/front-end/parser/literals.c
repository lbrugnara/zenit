#include <fllib.h>

#include "../../Test.h"
#include "../../../src/front-end/ast.h"
#include "../../../src/front-end/context.h"
#include "../../../src/front-end/lexer.h"
#include "../../../src/front-end/parse.h"
#include "tests.h"

void cenit_test_parser_literal_integer(void)
{
    const char *source = 
            "/* uint8 */ 1;                     \n"
            "/* uint8 */ 2;                     \n"
            "/* uint8 */ 3;                     \n"
            "/* uint8 */ 255;                   \n"
            //"/* uint64 */ 1844674407370955161;  \n"
    ;

    const size_t results[] = { 1, 2, 3, 255 };

    CenitContext ctx = cenit_context_new("global", CENIT_SOURCE_STRING, source);

    bool is_valid = cenit_parse_source(&ctx);

    fl_expect("Parser object should not contain errors", is_valid);

    for (size_t i=0; i < fl_array_length(ctx.ast->decls); i++)
    {
        CenitNode *node = ctx.ast->decls[i];
        fl_expect("Node must be a literal node", node->type == CENIT_NODE_LITERAL);

        CenitLiteralNode *literal = (CenitLiteralNode*)node;
        fl_vexpect(literal->typeinfo.type == CENIT_TYPE_UINT8, "Literal type must be \"%s\"", cenit_type_to_string(&literal->typeinfo));
        fl_vexpect((size_t)literal->value.uint8 == results[i], "Literal value must be equals to %zu", results[i]);
    }    

    cenit_context_free(&ctx);
}

void cenit_test_parser_literal_array_initializer(void)
{
    const char *source = 
            /* [3]uint8 */
            "[ 1, 2, 3 ];"      "\n"
            "[ ];"              "\n"
    ;

    const size_t elements[] = { 3, 0 };
    const size_t values[99][99] = { { 1, 2, 3 }, { 0 } };

    CenitContext ctx = cenit_context_new("global", CENIT_SOURCE_STRING, source);
    bool is_valid = cenit_parse_source(&ctx);

    fl_expect("Parser object should not contain errors", is_valid);

    for (size_t i=0; i < fl_array_length(ctx.ast->decls); i++)
    {
        CenitNode *node = ctx.ast->decls[i];
        fl_expect("Node must be a literal node", node->type == CENIT_NODE_ARRAY_INIT);

        CenitArrayInitNode *array = (CenitArrayInitNode*)node;

        fl_vexpect(array->values && fl_array_length(array->values) == elements[i], "Array must contain %zu elements", elements[i]);

        for (size_t j=0; j < fl_array_length(array->values); j++)
        {
            CenitLiteralNode *literal = (CenitLiteralNode*)array->values[j];
            fl_vexpect(literal->typeinfo.type == CENIT_TYPE_UINT8, "Literal type must be \"%s\"", cenit_type_to_string(&literal->typeinfo));
            fl_vexpect((size_t)literal->value.uint8 == values[i][j], "Literal value must be equals to %zu", values[i][j]);
        }
    }    

    cenit_context_free(&ctx);
}

void cenit_test_parser_literal_integer_error(void)
{
    const char *source = 
        /* ULLONG_MAX + 1: Too large integral type*/
        "18446744073709551616;          \n"
    ;

    CenitContext ctx = cenit_context_new("global", CENIT_SOURCE_STRING, source);
    bool is_valid = cenit_parse_source(&ctx);

    size_t expected_errors = 1;
    fl_vexpect(!is_valid && ctx.errors != NULL && fl_array_length(ctx.errors) == expected_errors, "The context object must contain %zu error(s)", expected_errors);

    CenitError *error = ctx.errors;

    fl_vexpect(error->location.line == 1 && error->type == CENIT_ERROR_LARGE_INTEGER, 
        "Expected semantic error: %s at line %u:%u", error->message, error->location.line, error->location.col);

    cenit_context_free(&ctx);
}
