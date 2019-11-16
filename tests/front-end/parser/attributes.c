#include <fllib.h>

#include "../../Test.h"
#include "../../../src/front-end/ast.h"
#include "../../../src/front-end/parse.h"
#include "tests.h"

static const char *node_str[] = {
    [ZENIT_NODE_IDENTIFIER] = "IDENTIFIER",
    [ZENIT_NODE_LITERAL]    = "LITERAL",
};

struct PropertyTest {
    const char *name;
    const enum ZenitNodeType node_type;
    const enum ZenitType type;
    const union {
        uint8_t uint8;
        uint16_t uint16;
        char *id;
    } value;
};

struct AttributeTest {
    const char *name;
    const size_t prop_count;
    struct PropertyTest properties[10];
};

struct VariableAttributesTest {
    const char *name;
    const size_t attr_count;
    struct AttributeTest attributes[10];
};

static struct VariableAttributesTest tests[] = {
    { 
        "variable", 4, 
            { 
                { "a1", 0, {0} }, 
                { "a2", 0, {0} }, 
                { "a3", 1, 
                    { 
                        { .name = "k", .node_type = ZENIT_NODE_IDENTIFIER, .value.id = "v" } 
                    } 
                },
                { "a4", 2, 
                    { 
                        { .name = "k", .node_type = ZENIT_NODE_IDENTIFIER, .value.id = "v" }, 
                        { .name = "k2", .node_type = ZENIT_NODE_IDENTIFIER, .value.id = "v2" } 
                    } 
                },
            }
    },
    { 
        "PPUCTRL", 1, 
            { 
                { "NES", 1, 
                    { 
                        { .name = "address", .node_type = ZENIT_NODE_LITERAL, .type = ZENIT_TYPE_UINT16, .value.uint16 = 8192 } 
                    } 
                },
            }
    },
    { 
        "spaceship", 1, 
            { 
                { "NES", 2, 
                    { 
                        { .name = "segment", .node_type = ZENIT_NODE_IDENTIFIER, .value.id = "zp" }, 
                        { .name = "address", .node_type = ZENIT_NODE_LITERAL, .type = ZENIT_TYPE_UINT8, .value.uint8 = 0 } 
                    } 
                },
            }
    }
};

void zenit_test_parser_attributes_variables(void)
{
    const char *source = 
        "#[a1]"                                 "\n"
        "#[a2()]"                               "\n"
        "#[a3(k: v)]"                           "\n"
        "#[a4(k: v, k2: v2)]"                   "\n"
        "var variable = 0;"                     "\n"

        "#[NES(address: 8192)]"                 "\n"
        "var PPUCTRL : uint8 = 1;"              "\n"
        "#[NES(segment: zp, address: 0)]"       "\n"
        "var spaceship : uint8 = 1;"            "\n"
    ;

    struct ZenitContext ctx = zenit_context_new(ZENIT_SOURCE_STRING, source);
    bool is_valid = zenit_parse_source(&ctx);

    size_t ntests = sizeof(tests) / sizeof(tests[0]);

    fl_vexpect(ctx.ast->decls != NULL && ntests == fl_array_length(ctx.ast->decls), "Number of parsed variables must be equals to %zu", ntests);

    for (size_t i=0; i < ntests; i++)
    {
        struct VariableAttributesTest *test = tests + i;
        struct ZenitVariableNode *vardecl = (struct ZenitVariableNode*)ctx.ast->decls[i];

        fl_vexpect(flm_cstring_equals(test->name, vardecl->name), "Variable name is expected to be '%s'", test->name);
        
        fl_vexpect(test->attr_count == zenit_attribute_node_map_length(&vardecl->attributes), 
            "Expected number of attributes in variable '%s' is %zu", test->name, test->attr_count);

        for (size_t j=0; j < test->attr_count; j++)
        {
            struct AttributeTest *attrtest = test->attributes + j;

            fl_vexpect(zenit_attribute_node_map_has_key(&vardecl->attributes, attrtest->name), "Attribute %s is expected to be present in the attributes list", attrtest->name);

            struct ZenitAttributeNode *attrnode = zenit_attribute_node_map_get(&vardecl->attributes, attrtest->name);

            fl_vexpect(flm_cstring_equals(attrtest->name, attrnode->name), "Attribute name is expected to be '%s'", attrtest->name);
        
            fl_vexpect(attrtest->prop_count == zenit_property_node_map_length(&attrnode->properties),
                "Expected number of properties in attribute '%s' is %zu", attrtest->name, attrtest->prop_count);

            if (attrtest->prop_count == 0)
                continue;

            for (size_t k=0; k < attrtest->prop_count; k++)
            {
                struct PropertyTest *proptest = attrtest->properties + k;

                fl_vexpect(zenit_property_node_map_has_key(&attrnode->properties, proptest->name), 
                    "Property %s is expected to be present", proptest->name);
                
                struct ZenitPropertyNode *propnode = zenit_property_node_map_get(&attrnode->properties, proptest->name);

                fl_vexpect(flm_cstring_equals(proptest->name, propnode->name), "Property name in attribute '%s' is expected to be '%s'", attrnode->name, proptest->name);
                fl_vexpect(proptest->node_type == propnode->value->type, "'%s' property's type in attribute '%s' is expected to be '%s'", proptest->name, attrnode->name, node_str[proptest->node_type]);
                
                if (proptest->node_type == ZENIT_NODE_IDENTIFIER)
                {
                    fl_vexpect(flm_cstring_equals(proptest->value.id, ((struct ZenitIdentifierNode*)propnode->value)->name), 
                        "'%s' property's value in attribute '%s' is expected to be '%s'", proptest->name, attrnode->name, proptest->value.id);
                }
                else if (proptest->node_type == ZENIT_NODE_LITERAL)
                {
                    bool is_valid = false;
                    uintmax_t test_value = 0;
                    uintmax_t prop_value = 0;

                    if (proptest->type == propnode->value->typeinfo.type)
                    {
                        switch (propnode->value->typeinfo.type)
                        {
                            case ZENIT_TYPE_UINT8:
                                test_value = proptest->value.uint8;
                                prop_value = ((struct ZenitLiteralNode*)propnode->value)->value.uint8;
                                is_valid = true;
                                break;
                            case ZENIT_TYPE_UINT16:
                                test_value = proptest->value.uint16;
                                prop_value = ((struct ZenitLiteralNode*)propnode->value)->value.uint16;
                                is_valid = true;
                                break;
                            default:
                                break;
                        }
                    }

                    fl_vexpect(is_valid && test_value == prop_value, "'%s' property's value in attribute '%s' is expected to be %ju", proptest->name, attrnode->name, test_value);
                }
            }
        }
    }

    zenit_context_free(&ctx);
}
