#include <fllib.h>

#include "../../Test.h"
#include "../../../src/front-end/ast/ast.h"
#include "../../../src/front-end/passes/parse.h"
#include "tests.h"

void zenit_test_parser_variable_literal(void)
{
    struct LiteralVariableTest {
        enum ZenitNodeType node_type;
        char *lhs_name;
        enum ZenitType type_declaration;
        enum ZenitNodeType rhs_node_type;
        enum ZenitUintTypeSize rhs_type_size;
        unsigned int rhs_value;
    } tests[] = {
        { ZENIT_NODE_VARIABLE, "num0", ZENIT_TYPE_NONE, ZENIT_NODE_UINT, ZENIT_UINT_8, 0 },
        { ZENIT_NODE_VARIABLE, "num1", ZENIT_TYPE_NONE, ZENIT_NODE_UINT, ZENIT_UINT_8, 1 },
        { ZENIT_NODE_VARIABLE, "num2", ZENIT_TYPE_NONE, ZENIT_NODE_UINT, ZENIT_UINT_8, 2 },
        { ZENIT_NODE_VARIABLE, "num3", ZENIT_TYPE_NONE, ZENIT_NODE_UINT, ZENIT_UINT_8, 3 },
    };

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
        struct LiteralVariableTest *test = tests + i;

        fl_vexpect(test->node_type == node->type, "Node type is %s", zenit_node_to_string(node));

        struct ZenitVariableNode *var_decl = (struct ZenitVariableNode*) node;

        fl_vexpect(flm_cstring_equals(test->lhs_name, var_decl->name), "Variable name must be equals to \"%s\"", test->lhs_name);
        fl_expect("Variable type is NONE because the type is inferred from the assignment", var_decl->type_decl == NULL);
        fl_expect("Right-hand side must be a literal node", var_decl->rvalue && var_decl->rvalue->type == test->rhs_node_type);

        struct ZenitUintNode *literal = (struct ZenitUintNode*) var_decl->rvalue;

        fl_expect("Right-hand side expression must be uint8", literal->size == test->rhs_type_size);
        fl_vexpect(literal->value.uint8 == test->rhs_value, "Right-hand side expression has value %zu", test->rhs_value);
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
        { ZENIT_NODE_UINT },
        { ZENIT_NODE_UINT },
        { ZENIT_NODE_UINT },
        { ZENIT_NODE_UINT, ZENIT_NODE_UINT }
    };

    const enum ZenitUintTypeSize sizes[10][10] = {
        { ZENIT_UINT_UNK },
        { ZENIT_UINT_8 }, 
        { ZENIT_UINT_8 }, 
        { ZENIT_UINT_8 }, 
        { ZENIT_UINT_8, ZENIT_UINT_8 } 
    };

    const char *sizes_names[10][10] = {
        { NULL },
        { "uint8" }, 
        { "uint8" }, 
        { "uint8" }, 
        { "uint8", "uint8" } 
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
        fl_expect("Variable type is NONE because the type is inferred from the assignment", var_decl->type_decl == NULL);
        fl_expect("Right-hand side must be an array initializer", var_decl->rvalue && var_decl->rvalue->type == ZENIT_NODE_ARRAY);

        struct ZenitArrayNode *array_init = (struct ZenitArrayNode*)var_decl->rvalue;        

        fl_vexpect(array_init->elements && fl_array_length(array_init->elements) == elements[i], "Number of elements in %s is %zu", name, elements[i]);

        for (size_t j=0; j < fl_array_length(array_init->elements); j++)
        {
            struct ZenitNode *value_node = array_init->elements[j];

            fl_vexpect(nodes[i][j] == value_node->type, "Array %s at position %zu is an integer literal", name, j);

            if (value_node->type == ZENIT_NODE_UINT)
            {
                struct ZenitUintNode *primitive_node = (struct ZenitUintNode*)value_node;
                fl_vexpect(
                    primitive_node->size == sizes[i][j], 
                    "Array %s at position %zu has type %s and value %d", 
                    name, 
                    j, 
                    sizes_names[i][j], 
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
        "var num4 : uint16 = 4;"
    ;

    const enum ZenitType var_types[10] = {
        ZENIT_TYPE_UINT,
        ZENIT_TYPE_UINT,
        ZENIT_TYPE_UINT,
        ZENIT_TYPE_UINT,
        ZENIT_TYPE_UINT,
    };

    const char *var_types_names[10] = {
        "uint8",
        "uint8", 
        "uint8",
        "uint8",
        "uint16",
    };

    const enum ZenitUintTypeSize literal_sizes[10] = {
        ZENIT_UINT_8,
        ZENIT_UINT_8,
        ZENIT_UINT_8,
        ZENIT_UINT_8,
        ZENIT_UINT_8,
    };
    
    const char *literal_sizes_names[10] = {
        "uint8",
        "uint8",
        "uint8",
        "uint8",
        "uint8",
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
        fl_vexpect(var_decl->type_decl->type == var_types[i], "Variable type is \"%s\" because the type is present in the declaration", var_types_names[i]);
        fl_expect("Right-hand side must be a literal node", var_decl->rvalue && var_decl->rvalue->type == ZENIT_NODE_UINT);

        struct ZenitUintNode *literal = (struct ZenitUintNode*)var_decl->rvalue;
        fl_vexpect(literal->size == literal_sizes[i], "Right-hand side expression must be of type \"%s\"", literal_sizes_names[i]);

        if (literal->size == ZENIT_UINT_8)
            fl_vexpect((size_t)literal->value.uint8 == i, "Right-hand side expression has value %zu", i);
        else if (literal->size == ZENIT_UINT_16)
            fl_vexpect((size_t)literal->value.uint16 == i, "Right-hand side expression has value %zu", i);
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
        "var num4 : [4]uint8 = [ 6, 7, 8, 9 ];"
        "var num5 : []uint8 = [ 10, 11, 12, 13, 14 ];"
    ;

    const size_t elements[] = { 0, 1, 2, 3, 4, 5 };
    const bool auto_elements[] = { false, false, false, false, false, true };

    const char *var_types_names[] = {
        "[0]uint8",
        "[1]uint8",
        "[2]uint8",
        "[3]uint8",
        "[4]uint8",
        "[]uint8",
    };

    const enum ZenitNodeType nodes[10][10] = {
        { (enum ZenitNodeType) -1 },
        { ZENIT_NODE_UINT },
        { ZENIT_NODE_UINT, ZENIT_NODE_UINT },
        { ZENIT_NODE_UINT, ZENIT_NODE_UINT, ZENIT_NODE_UINT },
        { ZENIT_NODE_UINT, ZENIT_NODE_UINT, ZENIT_NODE_UINT, ZENIT_NODE_UINT },
        { ZENIT_NODE_UINT, ZENIT_NODE_UINT, ZENIT_NODE_UINT, ZENIT_NODE_UINT, ZENIT_NODE_UINT }
    };

    const enum ZenitUintTypeSize sizes[10][10] = { 
        { ZENIT_UINT_UNK },
        { ZENIT_UINT_8 }, 
        { ZENIT_UINT_8, ZENIT_UINT_8 }, 
        { ZENIT_UINT_8, ZENIT_UINT_8, ZENIT_UINT_8 }, 
        { ZENIT_UINT_8, ZENIT_UINT_8, ZENIT_UINT_8, ZENIT_UINT_8 },
        { ZENIT_UINT_8, ZENIT_UINT_8, ZENIT_UINT_8, ZENIT_UINT_8, ZENIT_UINT_8 } 
    };

    const char *literal_sizes_names[10][10] = { 
        { NULL },
        { "uint8" }, 
        { "uint8", "uint8" }, 
        { "uint8", "uint8", "uint8" }, 
        { "uint8", "uint8", "uint8", "uint8" },
        { "uint8", "uint8", "uint8", "uint8", "uint8" } 
    };

    const int values[10][10] = { 
        { -1 },
        { 0 }, 
        { 1, 2 }, 
        { 3, 4, 5 }, 
        { 6, 7, 8, 9 },
        { 10, 11, 12, 13, 14 }
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

        if (auto_elements[i])
        {
            fl_vexpect(
                var_decl->type_decl->type == ZENIT_TYPE_ARRAY
                && ((struct ZenitArrayTypeNode*) var_decl->type_decl)->auto_length
                && ((struct ZenitArrayTypeNode*) var_decl->type_decl)->member_type->type == ZENIT_TYPE_UINT,
                "Variable type is array of uint8 with automatic length (\"%s\") because the type is present in the declaration, but the length is not", 
                var_types_names[i]);
        }
        else
        {
            fl_vexpect(
                var_decl->type_decl->type == ZENIT_TYPE_ARRAY
                && ((struct ZenitArrayTypeNode*) var_decl->type_decl)->length == elements[i]
                && ((struct ZenitArrayTypeNode*) var_decl->type_decl)->member_type->type == ZENIT_TYPE_UINT,
                "Variable type is array of %zu uint8 (\"%s\") because the type is present in the declaration", 
                elements[i],
                var_types_names[i]);
        }

        fl_expect("Right-hand side must be an array initializer", var_decl->rvalue && var_decl->rvalue->type == ZENIT_NODE_ARRAY);

        struct ZenitArrayNode *array_init = (struct ZenitArrayNode*)var_decl->rvalue;        

        fl_vexpect(array_init->elements && fl_array_length(array_init->elements) == elements[i], "Number of elements in %s is %zu (obtained from the rhs initializer)", name, elements[i]);

        for (size_t j=0; j < fl_array_length(array_init->elements); j++)
        {
            struct ZenitNode *value_node = array_init->elements[j];

            fl_vexpect(nodes[i][j] == value_node->type, "Array %s at position %zu is an integer literal", name, j);

            if (value_node->type == ZENIT_NODE_UINT)
            {
                struct ZenitUintNode *primitive_node = (struct ZenitUintNode*)value_node;
                fl_vexpect(
                    primitive_node->size == sizes[i][j], 
                    "Array %s at position %zu has type \"%s\" and value %d", 
                    name, 
                    j, 
                    literal_sizes_names[i][j], 
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
        fl_expect("Variable type is NONE because the type is inferred from the assignment", var_decl->type_decl == NULL);

        if (i == 0)
        {
            fl_expect("Right-hand side must be an identifier node", var_decl->rvalue && var_decl->rvalue->type == ZENIT_NODE_IDENTIFIER);

            struct ZenitIdentifierNode *identifier = (struct ZenitIdentifierNode*)var_decl->rvalue;
            fl_expect("Right-hand side expression must be identifier 'some_extern_id'", flm_cstring_equals(identifier->name, "some_extern_id"));
        }
        else if (i == 1)
        {
            fl_expect("Right-hand side must be an identifier node", var_decl->rvalue && var_decl->rvalue->type == ZENIT_NODE_IDENTIFIER);

            struct ZenitIdentifierNode *identifier = (struct ZenitIdentifierNode*)var_decl->rvalue;
            fl_expect("Right-hand side expression must be identifier 'num0'", flm_cstring_equals(identifier->name, "num0"));
        }
        else if (i == 2)
        {
            fl_expect("Right-hand side must be a reference operator node", var_decl->rvalue && var_decl->rvalue->type == ZENIT_NODE_REFERENCE);

            struct ZenitReferenceNode *ref_node = (struct ZenitReferenceNode*)var_decl->rvalue;
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

    const char *var_types_names[10] = {
        "uint8",
        "uint8",
        "&uint8",
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

        if (i == 0)
        {
            fl_vexpect(var_decl->type_decl->type == ZENIT_TYPE_UINT, "Variable type is \"%s\" because the type is present in the declaration", var_types_names[i]);
            fl_expect("Right-hand side must be an identifier node", var_decl->rvalue && var_decl->rvalue->type == ZENIT_NODE_IDENTIFIER);

            struct ZenitIdentifierNode *identifier = (struct ZenitIdentifierNode*)var_decl->rvalue;
            fl_expect("Right-hand side expression must be identifier 'some_extern_id'", flm_cstring_equals(identifier->name, "some_extern_id"));
        }
        else if (i == 1)
        {
            fl_vexpect(var_decl->type_decl->type == ZENIT_TYPE_UINT, "Variable type is \"%s\" because the type is present in the declaration", var_types_names[i]);
            fl_expect("Right-hand side must be an identifier node", var_decl->rvalue && var_decl->rvalue->type == ZENIT_NODE_IDENTIFIER);

            struct ZenitIdentifierNode *identifier = (struct ZenitIdentifierNode*)var_decl->rvalue;
            fl_expect("Right-hand side expression must be identifier 'num0'", flm_cstring_equals(identifier->name, "num0"));
        }
        else if (i == 2)
        {
            fl_vexpect(var_decl->type_decl->type == ZENIT_TYPE_REFERENCE 
                && ((struct ZenitReferenceTypeNode*) var_decl->type_decl)->element->type == ZENIT_TYPE_UINT, 
                "Variable type is \"%s\" because the type is present in the declaration", var_types_names[i]);
            fl_expect("Right-hand side must be a reference operator node", var_decl->rvalue && var_decl->rvalue->type == ZENIT_NODE_REFERENCE);

            struct ZenitReferenceNode *ref_node = (struct ZenitReferenceNode*)var_decl->rvalue;
            fl_expect("Right-hand side expression must be a reference node to an identifier", ref_node->expression->type == ZENIT_NODE_IDENTIFIER);

            struct ZenitIdentifierNode *identifier = (struct ZenitIdentifierNode*)ref_node->expression;
            fl_expect("Expression of the reference operator must be identifier 'num1'", flm_cstring_equals(identifier->name, "num1"));
        }
    }

    zenit_context_free(&ctx);
}
