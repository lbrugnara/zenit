#include <fllib.h>

#include "../../Test.h"
#include "../../../src/front-end/ast.h"
#include "../../../src/front-end/parse.h"
#include "tests.h"

void cenit_test_parser_variable(void)
{
    const char *source = 
        "var num0 = 0;"
        "var num1 = 1;"
        "var num2 = 2;"
        "var num3 = 3;";

    CenitContext ctx = cenit_context_new("global");
    bool is_valid = cenit_parse_string(&ctx, source);

    for (size_t i=0; i < fl_array_length(ctx.ast->decls); i++)
    {
        CenitNode *node = ctx.ast->decls[i];
        fl_expect("Node must be a variable declaration", node->type == CENIT_NODE_VARIABLE);

        CenitVariableNode *var_decl = (CenitVariableNode*)node;

        char name[5] = { 'n', 'u', 'm', (char)i + 48, '\0' };
        
        fl_vexpect(flm_cstring_equals(name, var_decl->name), "Variable name must be equals to \"%s\"", name);
        fl_expect("Variable type is NONE because the type is inferred from the assignment", var_decl->typeinfo.type == CENIT_TYPE_NONE);
        fl_expect("Right-hand side must be a literal node", var_decl->value && var_decl->value->type == CENIT_NODE_LITERAL);

        CenitLiteralNode *literal = (CenitLiteralNode*)var_decl->value;
        fl_expect("Right-hand side expression must have uint8", literal->typeinfo.type == CENIT_TYPE_UINT8);
        fl_vexpect((size_t)literal->value.uint8 == i, "Right-hand side expression has value %zu", i);
    }

    cenit_context_free(&ctx);
}

void cenit_test_parser_array_variable(void)
{
    const char *source = 
        "var num0 = [];"
        "var num1 = [ 0 ];"
        "var num2 = [ 1 ];"
        "var num3 = [ 2 ];"
        "var num4 = [ 3, 4 ];";

    const size_t elements[] = { 0, 1, 1, 1, 2 };

    const CenitNodeType nodes[10][10] = {
        { (CenitNodeType)-1 },
        { CENIT_NODE_LITERAL },
        { CENIT_NODE_LITERAL },
        { CENIT_NODE_LITERAL },
        { CENIT_NODE_LITERAL, CENIT_NODE_LITERAL }
    };

    const CenitType types[10][10] = {
        { (CenitType)-1 },
        { CENIT_TYPE_UINT8 }, 
        { CENIT_TYPE_UINT8 }, 
        { CENIT_TYPE_UINT8 }, 
        { CENIT_TYPE_UINT8, CENIT_TYPE_UINT8 } 
    };

    const int values[10][10] = { 
        { -1 },
        { 0 }, 
        { 1 }, 
        { 2 }, 
        { 3, 4 } 
    };

    CenitContext ctx = cenit_context_new("global");
    bool is_valid = cenit_parse_string(&ctx, source);

    for (size_t i=0; i < fl_array_length(ctx.ast->decls); i++)
    {
        CenitNode *node = ctx.ast->decls[i];
        fl_expect("Node must be a variable declaration", node->type == CENIT_NODE_VARIABLE);

        CenitVariableNode *var_decl = (CenitVariableNode*)node;

        char name[5] = { 'n', 'u', 'm', (char)i + 48, '\0' };
        
        fl_vexpect(flm_cstring_equals(name, var_decl->name), "Variable name must be equals to \"%s\"", name);
        fl_expect("Variable type is NONE because the type is inferred from the assignment", var_decl->typeinfo.type == CENIT_TYPE_NONE);
        fl_expect("Right-hand side must be an array initializer", var_decl->value && var_decl->value->type == CENIT_NODE_ARRAY_INIT);

        CenitArrayInitNode *array_init = (CenitArrayInitNode*)var_decl->value;        

        fl_vexpect(array_init->values && fl_array_length(array_init->values) == elements[i], "Number of elements in %s is %zu", name, elements[i]);

        for (size_t j=0; j < fl_array_length(array_init->values); j++)
        {
            CenitNode *value_node = array_init->values[j];

            fl_vexpect(nodes[i][j] == value_node->type, "Array %s at position %zu is an integer literal", name, j);

            if (value_node->type == CENIT_NODE_LITERAL)
            {
                CenitLiteralNode *literal_node = (CenitLiteralNode*)value_node;
                fl_vexpect(
                    literal_node->typeinfo.type == types[i][j], 
                    "Array %s at position %zu has type %s and value %d", 
                    name, 
                    j, 
                    cenit_type_string(literal_node->typeinfo.type), 
                    values[i][j]
                );
            }
        }
    }

    cenit_context_free(&ctx);
}

void cenit_test_parser_variable_type_info(void)
{
    const char *source = 
        "var num0 : uint8 = 0;"
        "var num1 : uint8 = 1;"
        "var num2 : uint8 = 2;"
        "var num3 : uint8 = 3;"
    ;

    CenitContext ctx = cenit_context_new("global");
    bool is_valid = cenit_parse_string(&ctx, source);

    for (size_t i=0; i < fl_array_length(ctx.ast->decls); i++)
    {
        CenitNode *node = ctx.ast->decls[i];
        fl_expect("Node must be a variable declaration", node->type == CENIT_NODE_VARIABLE);

        CenitVariableNode *var_decl = (CenitVariableNode*)node;

        char name[5] = { 'n', 'u', 'm', (char)i + 48, '\0' };
        
        fl_vexpect(flm_cstring_equals(name, var_decl->name), "Variable name must be equals to \"%s\"", name);
        fl_expect("Variable type is uint8 because the type is present in the declaration", var_decl->typeinfo.type == CENIT_TYPE_UINT8);
        fl_expect("Right-hand side must be a literal node", var_decl->value && var_decl->value->type == CENIT_NODE_LITERAL);

        CenitLiteralNode *literal = (CenitLiteralNode*)var_decl->value;
        fl_expect("Right-hand side expression must have uint8", literal->typeinfo.type == CENIT_TYPE_UINT8);
        fl_vexpect((size_t)literal->value.uint8 == i, "Right-hand side expression has value %zu", i);
    }

    cenit_context_free(&ctx);
}


void cenit_test_parser_array_variable_type_info(void)
{
    const char *source = 
        "var num0 : [0]uint8 = [];"
        "var num1 : [1]uint8 = [ 0 ];"
        "var num2 : [2]uint8 = [ 1, 2 ];"
        "var num3 : [3]uint8 = [ 3, 4, 5 ];"
        "var num4 : [4]uint8 = [ 6, 7, 8, 9 ];";

    const size_t elements[] = { 0, 1, 2, 3, 4 };

    const CenitNodeType nodes[10][10] = {
        { (CenitNodeType) -1 },
        { CENIT_NODE_LITERAL },
        { CENIT_NODE_LITERAL, CENIT_NODE_LITERAL },
        { CENIT_NODE_LITERAL, CENIT_NODE_LITERAL, CENIT_NODE_LITERAL },
        { CENIT_NODE_LITERAL, CENIT_NODE_LITERAL, CENIT_NODE_LITERAL, CENIT_NODE_LITERAL }
    };

    const CenitType types[10][10] = { 
        { (CenitType) -1 },
        { CENIT_TYPE_UINT8 }, 
        { CENIT_TYPE_UINT8, CENIT_TYPE_UINT8 }, 
        { CENIT_TYPE_UINT8, CENIT_TYPE_UINT8, CENIT_TYPE_UINT8 }, 
        { CENIT_TYPE_UINT8, CENIT_TYPE_UINT8, CENIT_TYPE_UINT8, CENIT_TYPE_UINT8 } 
    };

    const int values[10][10] = { 
        { -1 },
        { 0 }, 
        { 1, 2 }, 
        { 3, 4, 5 }, 
        { 6, 7, 8, 9 }
    };

    CenitContext ctx = cenit_context_new("global");
    bool is_valid = cenit_parse_string(&ctx, source);

    for (size_t i=0; i < fl_array_length(ctx.ast->decls); i++)
    {
        CenitNode *node = ctx.ast->decls[i];
        fl_expect("Node must be a variable declaration", node->type == CENIT_NODE_VARIABLE);

        CenitVariableNode *var_decl = (CenitVariableNode*)node;

        char name[5] = { 'n', 'u', 'm', (char)i + 48, '\0' };
        
        fl_vexpect(flm_cstring_equals(name, var_decl->name), "Variable name must be equals to \"%s\"", name);

        fl_vexpect(
            var_decl->typeinfo.is_array 
            && var_decl->typeinfo.elements == elements[i]
            && var_decl->typeinfo.type == CENIT_TYPE_UINT8,
            "Variable type is array of %zu uint8 because the type is present in the declaration", var_decl->typeinfo.elements);

        fl_expect("Right-hand side must be an array initializer", var_decl->value && var_decl->value->type == CENIT_NODE_ARRAY_INIT);

        CenitArrayInitNode *array_init = (CenitArrayInitNode*)var_decl->value;        

        fl_vexpect(array_init->values && fl_array_length(array_init->values) == elements[i], "Number of elements in %s is %zu", name, elements[i]);

        for (size_t j=0; j < fl_array_length(array_init->values); j++)
        {
            CenitNode *value_node = array_init->values[j];

            fl_vexpect(nodes[i][j] == value_node->type, "Array %s at position %zu is an integer literal", name, j);

            if (value_node->type == CENIT_NODE_LITERAL)
            {
                CenitLiteralNode *literal_node = (CenitLiteralNode*)value_node;
                fl_vexpect(
                    literal_node->typeinfo.type == types[i][j], 
                    "Array %s at position %zu has type %s and value %d", 
                    name, 
                    j, 
                    cenit_type_string(literal_node->typeinfo.type), 
                    values[i][j]
                );
            }
        }
    }

    cenit_context_free(&ctx);
}

void cenit_test_parser_variable_errors(void)
{
    const char *source = 
        /* The missing semicolon after the 0 makes the parser ignore everything up to finding the semicolon */
        "/* Line 1 */ var n : uint8 = 0   ignored by synch ;"                                              "\n"
        /* Missing variable name                                                                            */
        "/* Line 2 */ var : uint8 = 1;"                                                                    "\n"
        /* The missing semicolon after the 2 makes the parser ignore everything up to finding the semicolon */
        "/* Line 3 */ var n2 : uint8 = 2  ignored by synch ;"                                              "\n"
        /* Missing type name after the colon                                                                */
        "/* Line 4 */ var n3 : = 3;"                                                                       "\n"
        /* Missing assignment operator                                                                      */
        "/* Line 5 */ var n4 4;"                                                                           "\n"
        /* Missing variable initializer                                                                     */
        "/* Line 6 */ var n5 = ;"                                                                          "\n"
        /* ULLONG_MAX + 1: Too large integral type                                                          */
        "/* Line 7 */ var n6 = 18446744073709551616;"                                                      "\n"
        /* Array without initializer                                                                        */
        "/* Line 8 */ var n7 : [2]uint8 = ;"                                                               "\n"
        /* Invalid variable name                                                                            */
        "/* Line 9 */ var $invalid = 2;"                                                                   "\n"
    ;

    const CenitErrorType errors[] = {
        [0] = (CenitErrorType)-1,
        /* Line */ [1] = CENIT_ERROR_SYNTAX,
        /* Line */ [2] = CENIT_ERROR_SYNTAX,
        /* Line */ [3] = CENIT_ERROR_SYNTAX,
        /* Line */ [4] = CENIT_ERROR_SYNTAX,
        /* Line */ [5] = CENIT_ERROR_SYNTAX,
        /* Line */ [6] = CENIT_ERROR_SYNTAX,
        /* Line */ [7] = CENIT_ERROR_LARGE_INTEGER,
        /* Line */ [8] = CENIT_ERROR_SYNTAX,
        /* Line */ [9] = CENIT_ERROR_SYNTAX,
    };

    CenitContext ctx = cenit_context_new("global");

    bool is_valid = cenit_parse_string(&ctx, source);

    size_t expected_errors = (sizeof(errors) / sizeof(errors[0])) - 1;
    fl_vexpect(ctx.errors != NULL && fl_array_length(ctx.errors) == expected_errors, "The context object must contain %zu errors", expected_errors);

    CenitError *error = ctx.errors;

    for (size_t i=1; i <= expected_errors; i++, error++)
    {
        fl_vexpect(error->line == i && error->type == errors[i],
            "Expected %s error: %s at line %u:%u", errors[i] == error->type ? "syntax" : "large integer", error->message, error->line, error->col);
    }

    cenit_context_free(&ctx);
}
