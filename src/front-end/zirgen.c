#include "zirgen.h"
#include "program.h"
#include "symbol.h"
#include "../zir/program.h"
#include "../zir/value.h"

/*
 * Type: ZirGenerator
 *  An inferrer function takes a symbol from the <struct ZenitSymbolTable> and updates
 *  it -if needed- with type information the symbol could be missing.
 *  The function returns a <struct ZenitTypeInfo> with the type retrieved from the symbol
 *  or inferred from the context.
 */
typedef struct ZirOperand(*ZirGenerator)(struct ZenitContext *ctx, struct ZenitNode *node, struct ZirProgram *program);

// Visitor functions
static struct ZirOperand visit_node(struct ZenitContext *ctx, struct ZenitNode *node, struct ZirProgram *program);
static struct ZirOperand visit_primitive_node(struct ZenitContext *ctx, struct ZenitPrimitiveNode *primitive, struct ZirProgram *program);
static struct ZirOperand visit_variable_node(struct ZenitContext *ctx, struct ZenitVariableNode *vardecl, struct ZirProgram *program);
static struct ZirOperand visit_array_node(struct ZenitContext *ctx, struct ZenitArrayNode *array, struct ZirProgram *program);
static struct ZirOperand visit_identifier_node(struct ZenitContext *ctx, struct ZenitIdentifierNode *id_node, struct ZirProgram *program);
static struct ZirOperand visit_reference_node(struct ZenitContext *ctx, struct ZenitReferenceNode *ref_node, struct ZirProgram *program);
static struct ZirOperand visit_cast_node(struct ZenitContext *ctx, struct ZenitCastNode *cast_node, struct ZirProgram *program);

/*
 * Variable: generators
 *  An array indexed with a <enum ZenitNodeType> to get a <ZirGenerator> function
 */
static const ZirGenerator generators[] = {
    [ZENIT_NODE_LITERAL]    = (ZirGenerator) &visit_primitive_node,
    [ZENIT_NODE_VARIABLE]   = (ZirGenerator) &visit_variable_node,
    [ZENIT_NODE_ARRAY]      = (ZirGenerator) &visit_array_node,
    [ZENIT_NODE_IDENTIFIER] = (ZirGenerator) &visit_identifier_node,
    [ZENIT_NODE_REFERENCE]  = (ZirGenerator) &visit_reference_node,
    [ZENIT_NODE_CAST]       = (ZirGenerator) &visit_cast_node,
};


static inline void zenit_type_to_zir_type(struct ZenitTypeInfo *zenit_type, struct ZirTypeInfo *zir_type)
{
    switch (zenit_type->type)
    {
        case ZENIT_TYPE_UINT8:
            zir_type->type = ZIR_TYPE_UINT8;
            break;

        case ZENIT_TYPE_UINT16:
            zir_type->type = ZIR_TYPE_UINT16;
            break;

        case ZENIT_TYPE_CUSTOM:
            zir_type->type = ZIR_TYPE_CUSTOM;
            break;

        default:
            zir_type->type = ZIR_TYPE_NONE;
            break;
    }

    zir_type->elements = zenit_type->elements;
    zir_type->is_array = zenit_type->is_array;
    zir_type->is_ref = zenit_type->is_ref;
    zir_type->name = zenit_type->name;
}

static struct ZirAttributeMap zenit_attr_to_zir_attr(struct ZenitContext *ctx, struct ZirProgram *program, struct ZenitAttributeNodeMap zenit_attrs)
{
    // We always initialize the ZIR attributes map
    struct ZirAttributeMap zir_attrs = zir_attribute_map_new();

    // Get the Zenit attributes
    const char **zenit_attr_names = zenit_attribute_node_map_keys(&zenit_attrs);
    size_t zenit_attr_count = fl_array_length(zenit_attr_names);    

    for (size_t i=0; i < zenit_attr_count; i++)
    {
        // Get the Zenit attribute
        struct ZenitAttributeNode *zenit_attr = zenit_attribute_node_map_get(&zenit_attrs, zenit_attr_names[i]);
        // Get the Zenit properties
        const char **zenit_prop_names = zenit_property_node_map_keys(&zenit_attr->properties);
        size_t zenit_prop_count = fl_array_length(zenit_prop_names);

        // Create the ZIR attribute
        struct ZirAttribute *zir_attr = zir_attribute_new(zenit_attr->name);    

        // Create the ZIR properties (if any)
        for (size_t j=0; j < zenit_prop_count; j++)
        {
            // Get the Zenit property
            struct ZenitPropertyNode *zenit_prop = zenit_property_node_map_get(&zenit_attr->properties, zenit_prop_names[j]);

            // Visit the property's value to get the operand
            struct ZirOperand zir_operand = visit_node(ctx, zenit_prop->value, program);

            // Create the ZIR property with the operand
            struct ZirProperty *zir_prop = zir_property_new(zenit_prop->name, &zir_operand);

            // We add the parsed property to the attribute's properties map
            zir_property_map_add(&zir_attr->properties, zir_prop);
        }
        fl_array_free(zenit_prop_names);

        // Add the ZIR attribute to the map
        zir_attribute_map_add(&zir_attrs, zir_attr);
    }
    fl_array_free(zenit_attr_names);

    return zir_attrs;
}

static struct ZirOperand new_operand_symbol(struct ZenitContext *ctx, struct ZenitSymbol *symbol, struct ZirProgram *program, bool global_symbol)
{
    struct ZirTypeInfo zir_symbol_type = { 0 };
    zenit_type_to_zir_type(&symbol->typeinfo, &zir_symbol_type);
    
    struct ZirSymbol *zir_symbol = zir_symbol_new(symbol->name, &zir_symbol_type, false);

    if (global_symbol)
        zir_program_add_global_symbol(program, zir_symbol);
    else
        zir_program_add_symbol(program, zir_symbol);

    return (struct ZirOperand) {
        .type =  ZIR_OPERAND_SYMBOL,
        .operand.symbol = zir_symbol
    };
}

static struct ZirOperand new_operand_temp_symbol(struct ZenitContext *ctx,struct ZirProgram *program, bool global_symbol)
{
    char name[1024] = { 0 };
    snprintf(name, 1024, "tmp%llu", program->current->temp_counter++);

    struct ZirSymbol *zir_symbol = zir_symbol_new(name, NULL, true);

    if (global_symbol)
        zir_program_add_global_symbol(program, zir_symbol);
    else
        zir_program_add_symbol(program, zir_symbol);

    return (struct ZirOperand) {
        .type =  ZIR_OPERAND_SYMBOL,
        .operand.symbol = zir_symbol
    };
}

static struct ZirOperand visit_cast_node(struct ZenitContext *ctx, struct ZenitCastNode *cast_node, struct ZirProgram *program)
{
    // If it is an implicit cast (up cast), we let the back end manage it
    if (cast_node->implicit)
        return visit_node(ctx, cast_node->expression, program);
    
    // To down cast we need to issue an instruction
    struct ZirCastInstruction *cast_instr = zir_instruction_cast_new();

    // We use a temporal value to store the cast result
    cast_instr->lvalue = new_operand_temp_symbol(ctx, program, false);
    zenit_type_to_zir_type(&cast_node->base.typeinfo, &cast_instr->lvalue.operand.symbol->typeinfo);

    // We take the right operand from the casted expression
    cast_instr->rvalue = visit_node(ctx, cast_node->expression, program);

    // We add the instruction to the program
    zir_program_add_instruction(program, (struct ZirInstruction*)cast_instr);

    // The operand is the temporal value we created to store the cast's result
    return cast_instr->lvalue;
}

/*
 * Function: visit_primitive_node
 *  A primitive node is a constant value therefore it is a value operand.
 *
 * Parameters:
 *  ctx - Context object
 *  node - Literal node
 *  program - Program object
 *
 * Returns:
 *  struct ZirOperand - A constant value operand
 *
 */
static struct ZirOperand visit_primitive_node(struct ZenitContext *ctx, struct ZenitPrimitiveNode *primitive, struct ZirProgram *program)
{
    struct ZirLiteralValue *zir_primitive = (struct ZirLiteralValue*)zir_value_new(ZIR_VALUE_LITERAL);
    zenit_type_to_zir_type(&primitive->base.typeinfo, &zir_primitive->base.typeinfo);

    switch (primitive->base.typeinfo.type)
    {
        case ZENIT_TYPE_UINT8:
            zir_primitive->value.uint8 = primitive->value.uint8;
            break;
        case ZENIT_TYPE_UINT16:
            zir_primitive->value.uint16 = primitive->value.uint16;
            break;
        default:
            break;
    }

    return (struct ZirOperand) {
        .type = ZIR_OPERAND_VALUE,
        .operand.value = (struct ZirValue*)zir_primitive
    };
}

static struct ZirOperand visit_reference_node(struct ZenitContext *ctx, struct ZenitReferenceNode *ref_node, struct ZirProgram *program)
{
    struct ZirOperand operand = visit_node(ctx, ref_node->expression, program);
    operand.type = ZIR_OPERAND_REFERENCE;
    return operand;
}


/*
 * Function: visit_identifier_node
 *  Return an operand using the symbol of the identifier
 *
 * Parameters:
 *  ctx - Context object
 *  node - Literal node
 *  program - Program object
 *
 * Returns:
 *  struct ZirOperand - A symbol operand
 *
 */
static struct ZirOperand visit_identifier_node(struct ZenitContext *ctx, struct ZenitIdentifierNode *id_node, struct ZirProgram *program)
{
    return (struct ZirOperand) {
        .type = ZIR_OPERAND_SYMBOL,
        .operand.symbol = zir_symtable_get(&program->current->symtable, id_node->name)
    };
}

/*
 * Function: visit_array_node
 *  This function processes the array initializer to represent it as a value operand
 *  of an instruction
 *
 * Parameters:
 *  ctx - Context object
 *  node - Array initializer node
 *  program - Program object
 *
 * Returns:
 *  struct ZirOperand - An array value operand
 *
 */
static struct ZirOperand visit_array_node(struct ZenitContext *ctx, struct ZenitArrayNode *array, struct ZirProgram *program)
{
    struct ZirArrayValue *arrayval = (struct ZirArrayValue*)zir_value_new(ZIR_VALUE_ARRAY);
    zenit_type_to_zir_type(&array->base.typeinfo, &arrayval->base.typeinfo);

    arrayval->elements = fl_array_new(sizeof(struct ZirOperand), fl_array_length(array->elements));

    for (size_t i=0; i < fl_array_length(array->elements); i++)
        arrayval->elements[i] = visit_node(ctx, array->elements[i], program);

    return (struct ZirOperand) {
        .type = ZIR_OPERAND_VALUE,
        .operand.value = (struct ZirValue*)arrayval
    };
}

/*
 * Function: visit_variable_node
 *  This function creates a new instruction that declares a named variable in the current
 *  block scope.
 *
 * Parameters:
 *  ctx - Context object
 *  node - Variable declaration node
 *  program - Program object
 *
 * Returns:
 *  struct ZirOperand - The symbol operand that identifies the variable
 *
 */
static struct ZirOperand visit_variable_node(struct ZenitContext *ctx, struct ZenitVariableNode *vardecl, struct ZirProgram *program)
{
    // Get the <struct ZenitSymbol> object
    struct ZenitSymbol *symbol = zenit_program_get_symbol(ctx->program, vardecl->name);
    
    // Create the variable declaration instruction and fill its operands
    struct ZirVariableInstruction *varinstr = zir_instruction_variable_new();
    varinstr->attributes = zenit_attr_to_zir_attr(ctx, program, vardecl->attributes);

    // The lvalue is a symbol operand we create from the <struct ZenitSymbol>
    varinstr->lvalue = (struct ZirOperand) {
        .type = ZIR_OPERAND_SYMBOL,
        .operand.symbol = zir_symtable_get(&program->current->symtable, vardecl->name)
    };

    // The rvalue is the operand we get from the visit to the <struct ZenitVariableNode>'s 
    // value
    varinstr->rvalue = visit_node(ctx, vardecl->rvalue, program);

    // We add the instruction to the program
    zir_program_add_instruction(program, (struct ZirInstruction*)varinstr);

    return varinstr->lvalue;
}

/*
 * Function: visit_node
 *  This function selects the visitor function based on the node's type
 *  and calls the function.
 *
 * Parameters:
 *  ctx - Context object
 *  node - Node to visit
 *  program - Program object
 *
 * Returns:
 *  struct ZirOperand - A pointer to a an operand object
 *
 */
static struct ZirOperand visit_node(struct ZenitContext *ctx, struct ZenitNode *node, struct ZirProgram *program)
{
    return generators[node->type](ctx, node, program);
}

static void import_zir_symbols_from_zenit_symbols(struct ZenitContext *ctx, struct ZirProgram *program)
{
    // FIXME: Use a zenit_program function to get all the symbols
    // FIXME: Use the scopes!
    char **names = fl_hashtable_keys(ctx->program->global_scope->symtable.symbols);

    for (size_t i=0; i < fl_array_length(names); i++)
    {
        struct ZenitSymbol *symbol = zenit_program_get_symbol(ctx->program, names[i]);
        new_operand_symbol(ctx, symbol, program, ctx->program->global_scope->symtable.type == ZENIT_SYMTABLE_GLOBAL);
    }

    fl_array_free(names);
}

/*
 * Function: zenit_generate_zir
 *  We just iterate over the declarations visiting each node to populate the <struct ZirProgram>
 *  with <struct ZirInstruction>s
 */
struct ZirProgram* zenit_generate_zir(struct ZenitContext *ctx)
{
    if (!ctx || !ctx->ast || !ctx->ast->decls)
        return NULL;

    struct ZirProgram *program = zir_program_new();
    import_zir_symbols_from_zenit_symbols(ctx, program);

    size_t errors = zenit_context_error_count(ctx);

    for (size_t i=0; i < fl_array_length(ctx->ast->decls); i++)
        visit_node(ctx, ctx->ast->decls[i], program);

    if (errors == zenit_context_error_count(ctx))
        return program;

    zir_program_free(program);
    return NULL;
}
