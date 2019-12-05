#include <fllib.h>

#include "../../Test.h"
#include "../../../src/front-end/ast.h"
#include "../../../src/front-end/parse.h"
#include "tests.h"

void zenit_test_parser_variable_literal(void)
{
    const char *source = 
        "var num0 = 0;"
        "var num1 = 1;"
        "var num2 = 2;"
        "var num3 = 3;";

    struct ZenitContext ctx = zenit_context_new(ZENIT_SOURCE_STRING, source);
    bool is_valid = zenit_parse_source(&ctx);

    for (size_t i=0; i < fl_array_length(ctx.ast->decls); i++)
    {
        struct ZenitNode *node = ctx.ast->decls[i];
        fl_expect("Node must be a variable declaration", node->type == ZENIT_NODE_VARIABLE);

        struct ZenitVariableNode *var_decl = (struct ZenitVariableNode*)node;

        char name[5] = { 'n', 'u', 'm', (char)i + 48, '\0' };
        
        fl_vexpect(flm_cstring_equals(name, var_decl->name), "Variable name must be equals to \"%s\"", name);
        fl_expect("Variable type is NONE because the type is inferred from the assignment", var_decl->base.typeinfo.type == ZENIT_TYPE_NONE);
        fl_expect("Right-hand side must be a literal node", var_decl->value && var_decl->value->type == ZENIT_NODE_LITERAL);

        struct ZenitPrimitiveNode *literal = (struct ZenitPrimitiveNode*)var_decl->value;
        fl_expect("Right-hand side expression must have uint8", literal->base.typeinfo.type == ZENIT_TYPE_UINT8);
        fl_vexpect((size_t)literal->value.uint8 == i, "Right-hand side expression has value %zu", i);
    }

    zenit_context_free(&ctx);
}

void zenit_test_parser_array_variable_literal(void)
{
    const char *source = 
        "var num0 = [];"
        "var num1 = [ 0 ];"
        "var num2 = [ 1 ];"
        "var num3 = [ 2 ];"
        "var num4 = [ 3, 4 ];";

    const size_t elements[] = { 0, 1, 1, 1, 2 };

    const enum ZenitNodeType nodes[10][10] = {
        { (enum ZenitNodeType)-1 },
        { ZENIT_NODE_LITERAL },
        { ZENIT_NODE_LITERAL },
        { ZENIT_NODE_LITERAL },
        { ZENIT_NODE_LITERAL, ZENIT_NODE_LITERAL }
    };

    const enum ZenitType types[10][10] = {
        { (enum ZenitType)-1 },
        { ZENIT_TYPE_UINT8 }, 
        { ZENIT_TYPE_UINT8 }, 
        { ZENIT_TYPE_UINT8 }, 
        { ZENIT_TYPE_UINT8, ZENIT_TYPE_UINT8 } 
    };

    const int values[10][10] = { 
        { -1 },
        { 0 }, 
        { 1 }, 
        { 2 }, 
        { 3, 4 } 
    };

    struct ZenitContext ctx = zenit_context_new(ZENIT_SOURCE_STRING, source);
    bool is_valid = zenit_parse_source(&ctx);

    for (size_t i=0; i < fl_array_length(ctx.ast->decls); i++)
    {
        struct ZenitNode *node = ctx.ast->decls[i];
        fl_expect("Node must be a variable declaration", node->type == ZENIT_NODE_VARIABLE);

        struct ZenitVariableNode *var_decl = (struct ZenitVariableNode*)node;

        char name[5] = { 'n', 'u', 'm', (char)i + 48, '\0' };
        
        fl_vexpect(flm_cstring_equals(name, var_decl->name), "Variable name must be equals to \"%s\"", name);
        fl_expect("Variable type is NONE because the type is inferred from the assignment", var_decl->base.typeinfo.type == ZENIT_TYPE_NONE);
        fl_expect("Right-hand side must be an array initializer", var_decl->value && var_decl->value->type == ZENIT_NODE_ARRAY);

        struct ZenitArrayNode *array_init = (struct ZenitArrayNode*)var_decl->value;        

        fl_vexpect(array_init->elements && fl_array_length(array_init->elements) == elements[i], "Number of elements in %s is %zu", name, elements[i]);

        for (size_t j=0; j < fl_array_length(array_init->elements); j++)
        {
            struct ZenitNode *value_node = array_init->elements[j];

            fl_vexpect(nodes[i][j] == value_node->type, "Array %s at position %zu is an integer literal", name, j);

            if (value_node->type == ZENIT_NODE_LITERAL)
            {
                struct ZenitPrimitiveNode *primitive_node = (struct ZenitPrimitiveNode*)value_node;
                fl_vexpect(
                    primitive_node->base.typeinfo.type == types[i][j], 
                    "Array %s at position %zu has type %s and value %d", 
                    name, 
                    j, 
                    zenit_type_to_string(&primitive_node->base.typeinfo), 
                    values[i][j]
                );
            }
        }
    }

    zenit_context_free(&ctx);
}

void zenit_test_parser_variable_literal_typeinfo(void)
{
    const char *source = 
        "var num0 : uint8 = 0;"
        "var num1 : uint8 = 1;"
        "var num2 : uint8 = 2;"
        "var num3 : uint8 = 3;"
    ;

    struct ZenitContext ctx = zenit_context_new(ZENIT_SOURCE_STRING, source);
    bool is_valid = zenit_parse_source(&ctx);

    for (size_t i=0; i < fl_array_length(ctx.ast->decls); i++)
    {
        struct ZenitNode *node = ctx.ast->decls[i];
        fl_expect("Node must be a variable declaration", node->type == ZENIT_NODE_VARIABLE);

        struct ZenitVariableNode *var_decl = (struct ZenitVariableNode*)node;

        char name[5] = { 'n', 'u', 'm', (char)i + 48, '\0' };
        
        fl_vexpect(flm_cstring_equals(name, var_decl->name), "Variable name must be equals to \"%s\"", name);
        fl_vexpect(var_decl->base.typeinfo.type == ZENIT_TYPE_UINT8, "Variable type is \"%s\" because the type is present in the declaration", zenit_type_to_string(&var_decl->base.typeinfo));
        fl_expect("Right-hand side must be a literal node", var_decl->value && var_decl->value->type == ZENIT_NODE_LITERAL);

        struct ZenitPrimitiveNode *literal = (struct ZenitPrimitiveNode*)var_decl->value;
        fl_vexpect(literal->base.typeinfo.type == ZENIT_TYPE_UINT8, "Right-hand side expression must be of type \"%s\"", zenit_type_to_string(&literal->base.typeinfo));
        fl_vexpect((size_t)literal->value.uint8 == i, "Right-hand side expression has value %zu", i);
    }

    zenit_context_free(&ctx);
}


void zenit_test_parser_array_variable_literal_typeinfo(void)
{
    const char *source = 
        "var num0 : [0]uint8 = [];"
        "var num1 : [1]uint8 = [ 0 ];"
        "var num2 : [2]uint8 = [ 1, 2 ];"
        "var num3 : [3]uint8 = [ 3, 4, 5 ];"
        "var num4 : [4]uint8 = [ 6, 7, 8, 9 ];";

    const size_t elements[] = { 0, 1, 2, 3, 4 };

    const enum ZenitNodeType nodes[10][10] = {
        { (enum ZenitNodeType) -1 },
        { ZENIT_NODE_LITERAL },
        { ZENIT_NODE_LITERAL, ZENIT_NODE_LITERAL },
        { ZENIT_NODE_LITERAL, ZENIT_NODE_LITERAL, ZENIT_NODE_LITERAL },
        { ZENIT_NODE_LITERAL, ZENIT_NODE_LITERAL, ZENIT_NODE_LITERAL, ZENIT_NODE_LITERAL }
    };

    const enum ZenitType types[10][10] = { 
        { (enum ZenitType) -1 },
        { ZENIT_TYPE_UINT8 }, 
        { ZENIT_TYPE_UINT8, ZENIT_TYPE_UINT8 }, 
        { ZENIT_TYPE_UINT8, ZENIT_TYPE_UINT8, ZENIT_TYPE_UINT8 }, 
        { ZENIT_TYPE_UINT8, ZENIT_TYPE_UINT8, ZENIT_TYPE_UINT8, ZENIT_TYPE_UINT8 } 
    };

    const int values[10][10] = { 
        { -1 },
        { 0 }, 
        { 1, 2 }, 
        { 3, 4, 5 }, 
        { 6, 7, 8, 9 }
    };

    struct ZenitContext ctx = zenit_context_new(ZENIT_SOURCE_STRING, source);
    bool is_valid = zenit_parse_source(&ctx);

    for (size_t i=0; i < fl_array_length(ctx.ast->decls); i++)
    {
        struct ZenitNode *node = ctx.ast->decls[i];
        fl_expect("Node must be a variable declaration", node->type == ZENIT_NODE_VARIABLE);

        struct ZenitVariableNode *var_decl = (struct ZenitVariableNode*)node;

        char name[5] = { 'n', 'u', 'm', (char)i + 48, '\0' };
        
        fl_vexpect(flm_cstring_equals(name, var_decl->name), "Variable name must be equals to \"%s\"", name);

        fl_vexpect(
            var_decl->base.typeinfo.is_array 
            && var_decl->base.typeinfo.elements == elements[i]
            && var_decl->base.typeinfo.type == ZENIT_TYPE_UINT8,
            "Variable type is array of %zu uint8 (\"%s\") because the type is present in the declaration", 
            var_decl->base.typeinfo.elements,
            zenit_type_to_string(&var_decl->base.typeinfo));

        fl_expect("Right-hand side must be an array initializer", var_decl->value && var_decl->value->type == ZENIT_NODE_ARRAY);

        struct ZenitArrayNode *array_init = (struct ZenitArrayNode*)var_decl->value;        

        fl_vexpect(array_init->elements && fl_array_length(array_init->elements) == elements[i], "Number of elements in %s is %zu", name, elements[i]);

        for (size_t j=0; j < fl_array_length(array_init->elements); j++)
        {
            struct ZenitNode *value_node = array_init->elements[j];

            fl_vexpect(nodes[i][j] == value_node->type, "Array %s at position %zu is an integer literal", name, j);

            if (value_node->type == ZENIT_NODE_LITERAL)
            {
                struct ZenitPrimitiveNode *primitive_node = (struct ZenitPrimitiveNode*)value_node;
                fl_vexpect(
                    primitive_node->base.typeinfo.type == types[i][j], 
                    "Array %s at position %zu has type \"%s\" and value %d", 
                    name, 
                    j, 
                    zenit_type_to_string(&primitive_node->base.typeinfo), 
                    values[i][j]
                );
            }
        }
    }

    zenit_context_free(&ctx);
}

void zenit_test_parser_variable_complex(void)
{
    const char *source = 
        "var num0 = some_extern_id;"
        "var num1 = num0;"
        "var num2 = &num1;"
    ;

    struct ZenitContext ctx = zenit_context_new(ZENIT_SOURCE_STRING, source);
    bool is_valid = zenit_parse_source(&ctx);

    for (size_t i=0; i < fl_array_length(ctx.ast->decls); i++)
    {
        struct ZenitNode *node = ctx.ast->decls[i];
        fl_expect("Node must be a variable declaration", node->type == ZENIT_NODE_VARIABLE);

        struct ZenitVariableNode *var_decl = (struct ZenitVariableNode*)node;

        char name[5] = { 'n', 'u', 'm', (char)i + 48, '\0' };
        
        fl_vexpect(flm_cstring_equals(name, var_decl->name), "Variable name must be equals to \"%s\"", name);
        fl_expect("Variable type is NONE because the type is inferred from the assignment", var_decl->base.typeinfo.type == ZENIT_TYPE_NONE);

        if (i == 0)
        {
            fl_expect("Right-hand side must be an identifier node", var_decl->value && var_decl->value->type == ZENIT_NODE_IDENTIFIER);

            struct ZenitIdentifierNode *identifier = (struct ZenitIdentifierNode*)var_decl->value;
            fl_expect("Right-hand side expression must be identifier 'some_extern_id'", flm_cstring_equals(identifier->name, "some_extern_id"));
        }
        else if (i == 1)
        {
            fl_expect("Right-hand side must be an identifier node", var_decl->value && var_decl->value->type == ZENIT_NODE_IDENTIFIER);

            struct ZenitIdentifierNode *identifier = (struct ZenitIdentifierNode*)var_decl->value;
            fl_expect("Right-hand side expression must be identifier 'num0'", flm_cstring_equals(identifier->name, "num0"));
        }
        else if (i == 2)
        {
            fl_expect("Right-hand side must be a reference operator node", var_decl->value && var_decl->value->type == ZENIT_NODE_REFERENCE);

            struct ZenitReferenceNode *ref_node = (struct ZenitReferenceNode*)var_decl->value;
            fl_expect("Right-hand side expression must be a reference node to an identifier", ref_node->expression->type == ZENIT_NODE_IDENTIFIER);

            struct ZenitIdentifierNode *identifier = (struct ZenitIdentifierNode*)ref_node->expression;
            fl_expect("Expression of the reference operator must be identifier 'num1'", flm_cstring_equals(identifier->name, "num1"));
        }
    }

    zenit_context_free(&ctx);
}

void zenit_test_parser_variable_complex_typeinfo(void)
{
    const char *source = 
        "var num0 : uint8 = some_extern_id;"
        "var num1 : uint8 = num0;"
        "var num2 : &uint8 = &num1;"
    ;

    struct ZenitContext ctx = zenit_context_new(ZENIT_SOURCE_STRING, source);
    bool is_valid = zenit_parse_source(&ctx);

    for (size_t i=0; i < fl_array_length(ctx.ast->decls); i++)
    {
        struct ZenitNode *node = ctx.ast->decls[i];
        fl_expect("Node must be a variable declaration", node->type == ZENIT_NODE_VARIABLE);

        struct ZenitVariableNode *var_decl = (struct ZenitVariableNode*)node;

        char name[5] = { 'n', 'u', 'm', (char)i + 48, '\0' };
        
        fl_vexpect(flm_cstring_equals(name, var_decl->name), "Variable name must be equals to \"%s\"", name);

        if (i == 0)
        {
            fl_vexpect(var_decl->base.typeinfo.type == ZENIT_TYPE_UINT8, "Variable type is \"%s\" because the type is present in the declaration", zenit_type_to_string(&var_decl->base.typeinfo));
            fl_expect("Right-hand side must be an identifier node", var_decl->value && var_decl->value->type == ZENIT_NODE_IDENTIFIER);

            struct ZenitIdentifierNode *identifier = (struct ZenitIdentifierNode*)var_decl->value;
            fl_expect("Right-hand side expression must be identifier 'some_extern_id'", flm_cstring_equals(identifier->name, "some_extern_id"));
        }
        else if (i == 1)
        {
            fl_vexpect(var_decl->base.typeinfo.type == ZENIT_TYPE_UINT8, "Variable type is \"%s\" because the type is present in the declaration", zenit_type_to_string(&var_decl->base.typeinfo));
            fl_expect("Right-hand side must be an identifier node", var_decl->value && var_decl->value->type == ZENIT_NODE_IDENTIFIER);

            struct ZenitIdentifierNode *identifier = (struct ZenitIdentifierNode*)var_decl->value;
            fl_expect("Right-hand side expression must be identifier 'num0'", flm_cstring_equals(identifier->name, "num0"));
        }
        else if (i == 2)
        {
            fl_vexpect(var_decl->base.typeinfo.type == ZENIT_TYPE_UINT8 && var_decl->base.typeinfo.is_ref, 
                "Variable type is \"%s\" because the type is present in the declaration", zenit_type_to_string(&var_decl->base.typeinfo));
            fl_expect("Right-hand side must be a reference operator node", var_decl->value && var_decl->value->type == ZENIT_NODE_REFERENCE);

            struct ZenitReferenceNode *ref_node = (struct ZenitReferenceNode*)var_decl->value;
            fl_expect("Right-hand side expression must be a reference node to an identifier", ref_node->expression->type == ZENIT_NODE_IDENTIFIER);

            struct ZenitIdentifierNode *identifier = (struct ZenitIdentifierNode*)ref_node->expression;
            fl_expect("Expression of the reference operator must be identifier 'num1'", flm_cstring_equals(identifier->name, "num1"));
        }
    }

    zenit_context_free(&ctx);
}
