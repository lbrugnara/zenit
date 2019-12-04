#include "zirgen.h"
#include "program.h"
#include "symbol.h"
#include "../zir/program.h"
#include "../zir/value.h"

/*
 * Type: ZenitIrGenerator
 *  An inferrer function takes a symbol from the <struct ZenitSymbolTable> and updates
 *  it -if needed- with type information the symbol could be missing.
 *  The function returns a <struct ZenitTypeInfo> with the type retrieved from the symbol
 *  or inferred from the context.
 */
typedef struct ZenitIrOperand(*ZenitIrGenerator)(struct ZenitContext *ctx, struct ZenitNode *node, struct ZenitIrProgram *program);

// Visitor functions
static struct ZenitIrOperand visit_node(struct ZenitContext *ctx, struct ZenitNode *node, struct ZenitIrProgram *program);
static struct ZenitIrOperand visit_literal(struct ZenitContext *ctx, struct ZenitNode *node, struct ZenitIrProgram *program);
static struct ZenitIrOperand visit_variable(struct ZenitContext *ctx, struct ZenitNode *node, struct ZenitIrProgram *program);
static struct ZenitIrOperand visit_array_initializer(struct ZenitContext *ctx, struct ZenitNode *node, struct ZenitIrProgram *program);
static struct ZenitIrOperand visit_identifier(struct ZenitContext *ctx, struct ZenitNode *node, struct ZenitIrProgram *program);
static struct ZenitIrOperand visit_unary_ref(struct ZenitContext *ctx, struct ZenitNode *node, struct ZenitIrProgram *program);
static struct ZenitIrOperand visit_cast(struct ZenitContext *ctx, struct ZenitNode *node, struct ZenitIrProgram *program);

/*
 * Variable: generators
 *  An array indexed with a <enum ZenitNodeType> to get a <ZenitIrGenerator> function
 */
static const ZenitIrGenerator generators[] = {
    [ZENIT_NODE_LITERAL]    = &visit_literal,
    [ZENIT_NODE_VARIABLE]   = &visit_variable,
    [ZENIT_NODE_ARRAY_INIT] = &visit_array_initializer,
    [ZENIT_NODE_IDENTIFIER] = &visit_identifier,
    [ZENIT_NODE_UNARY_REF]  = &visit_unary_ref,
    [ZENIT_NODE_CAST]       = &visit_cast,
};


static void zenit_type_to_zenit_ir_type(struct ZenitTypeInfo *zenit_type, struct ZenitIrTypeInfo *zenit_ir_type)
{
    switch (zenit_type->type)
    {
        case ZENIT_TYPE_UINT8:
            zenit_ir_type->type = ZENIT_IR_TYPE_UINT8;
            break;

        case ZENIT_TYPE_UINT16:
            zenit_ir_type->type = ZENIT_IR_TYPE_UINT16;
            break;

        case ZENIT_TYPE_CUSTOM:
            zenit_ir_type->type = ZENIT_IR_TYPE_CUSTOM;
            break;

        default:
            zenit_ir_type->type = ZENIT_IR_TYPE_NONE;
            break;
    }

    zenit_ir_type->elements = zenit_type->elements;
    zenit_ir_type->is_array = zenit_type->is_array;
    zenit_ir_type->is_ref = zenit_type->is_ref;
    zenit_ir_type->name = zenit_type->name;
}

struct ZenitIrAttributeMap zenit_attr_to_zenit_ir_attr(struct ZenitContext *ctx, struct ZenitIrProgram *program, struct ZenitAttributeNodeMap zenit_attrs)
{
    // We always initialize the ZIR attributes map
    struct ZenitIrAttributeMap zir_attrs = zenit_ir_attribute_map_new();

    // Get the Zenit attributes
    const char **zenit_attr_names = zenit_attribute_node_map_keys(&zenit_attrs);
    size_t zenit_attr_count = fl_array_length(zenit_attr_names);    

    for (size_t i=0; i < zenit_attr_count; i++)
    {
        // Get the Zenit attribute
        struct ZenitAttributeNode *zenit_attr = zenit_attribute_node_map_get(&zenit_attrs, zenit_attr_names[i]);
        
        // Create the ZIR attribute
        struct ZenitIrAttribute *zir_attr = fl_malloc(sizeof(struct ZenitIrAttribute));
        zir_attr->name = fl_cstring_dup(zenit_attr->name);
        // We always initialize the ZIR attribute properties
        zir_attr->properties = zenit_ir_property_map_new();
        
        // Get the Zenit properties
        const char **zenit_prop_names = zenit_property_node_map_keys(&zenit_attr->properties);
        size_t zenit_prop_count = fl_array_length(zenit_prop_names);

        for (size_t j=0; j < zenit_prop_count; j++)
        {
            // Get the Zenit property
            struct ZenitPropertyNode *zenit_prop = zenit_property_node_map_get(&zenit_attr->properties, zenit_prop_names[j]);

            // Create the ZIR property
            struct ZenitIrProperty *zir_prop = fl_malloc(sizeof(struct ZenitIrProperty));
            zir_prop->name = fl_cstring_dup(zenit_prop->name);

            // We need to generate the operand
            struct ZenitIrOperand zir_operand = visit_node(ctx, zenit_prop->value, program);
            memcpy(&zir_prop->value, &zir_operand, sizeof(struct ZenitIrOperand));

            // We add the parsed property
            zenit_ir_property_map_add(&zir_attr->properties, zir_prop);
        }

        fl_array_free(zenit_prop_names);

        zenit_ir_attribute_map_add(&zir_attrs, zir_attr);
    }

    fl_array_free(zenit_attr_names);

    return zir_attrs;
}

static struct ZenitIrOperand new_operand_symbol(struct ZenitContext *ctx, struct ZenitSymbol *symbol, struct ZenitIrProgram *program, bool global_symbol)
{
    struct ZenitIrTypeInfo zenit_ir_symbol_type = { 0 };
    zenit_type_to_zenit_ir_type(&symbol->typeinfo, &zenit_ir_symbol_type);
    
    struct ZenitIrSymbol *zenit_ir_symbol = zenit_ir_symbol_new(symbol->name, &zenit_ir_symbol_type, false);

    if (global_symbol)
        zenit_ir_program_add_global_symbol(program, zenit_ir_symbol);
    else
        zenit_ir_program_add_symbol(program, zenit_ir_symbol);

    return (struct ZenitIrOperand) {
        .type =  ZENIT_IR_OPERAND_SYMBOL,
        .operand.symbol = zenit_ir_symbol
    };
}

static struct ZenitIrOperand new_operand_temp_symbol(struct ZenitContext *ctx,struct ZenitIrProgram *program, bool global_symbol)
{
    char name[1024] = { 0 };
    snprintf(name, 1024, "tmp%llu", program->current->temp_counter++);

    struct ZenitIrSymbol *zenit_ir_symbol = zenit_ir_symbol_new(name, NULL, true);

    if (global_symbol)
        zenit_ir_program_add_global_symbol(program, zenit_ir_symbol);
    else
        zenit_ir_program_add_symbol(program, zenit_ir_symbol);

    return (struct ZenitIrOperand) {
        .type =  ZENIT_IR_OPERAND_SYMBOL,
        .operand.symbol = zenit_ir_symbol
    };
}

static struct ZenitIrOperand visit_cast(struct ZenitContext *ctx, struct ZenitNode *node, struct ZenitIrProgram *program)
{
    struct ZenitCastNode *cast_node = (struct ZenitCastNode*)node;

    // If it is an implicit cast (up cast), we let the back end manage it
    if (cast_node->implicit)
        return visit_node(ctx, cast_node->expression, program);
    
    // To down cast we need to issue an instruction
    struct ZenitIrCastInstruction *cast_instr = zenit_ir_instruction_cast_new();

    // We use a temporal value to store the cast result
    cast_instr->lvalue = new_operand_temp_symbol(ctx, program, false);
    zenit_type_to_zenit_ir_type(&cast_node->base.typeinfo, &cast_instr->lvalue.operand.symbol->typeinfo);

    // We take the right operand from the casted expression
    cast_instr->rvalue = visit_node(ctx, cast_node->expression, program);

    // We add the instruction to the program
    zenit_ir_program_add_instruction(program, (struct ZenitIrInstruction*)cast_instr);

    // The operand is the temporal value we created to store the cast's result
    return cast_instr->lvalue;
}

/*
 * Function: visit_literal
 *  A literal node is a constant value therefore it is a value operand.
 *
 * Parameters:
 *  ctx - Context object
 *  node - Literal node
 *  program - Program object
 *
 * Returns:
 *  struct ZenitIrOperand - A constant value operand
 *
 */
static struct ZenitIrOperand visit_literal(struct ZenitContext *ctx, struct ZenitNode *node, struct ZenitIrProgram *program)
{
    struct ZenitLiteralNode *literal = (struct ZenitLiteralNode*)node;

    struct ZenitIrLiteralValue *literalval = (struct ZenitIrLiteralValue*)zenit_ir_value_new(ZENIT_IR_VALUE_LITERAL);
    zenit_type_to_zenit_ir_type(&literal->base.typeinfo, &literalval->base.typeinfo);

    switch (literal->base.typeinfo.type)
    {
        case ZENIT_TYPE_UINT8:
            literalval->value.uint8 = literal->value.uint8;
            break;
        case ZENIT_TYPE_UINT16:
            literalval->value.uint16 = literal->value.uint16;
            break;
        default:
            break;
    }

    return (struct ZenitIrOperand) {
        .type = ZENIT_IR_OPERAND_VALUE,
        .operand.value = (struct ZenitIrValue*)literalval
    };
}

static struct ZenitIrOperand visit_unary_ref(struct ZenitContext *ctx, struct ZenitNode *node, struct ZenitIrProgram *program)
{
    struct ZenitUnaryRefNode *ref_node = (struct ZenitUnaryRefNode*)node;
    struct ZenitIrOperand operand = visit_node(ctx, ref_node->expression, program);
    operand.type = ZENIT_IR_OPERAND_REF;
    return operand;
}


/*
 * Function: visit_identifier
 *  Return an operand using the symbol of the identifier
 *
 * Parameters:
 *  ctx - Context object
 *  node - Literal node
 *  program - Program object
 *
 * Returns:
 *  struct ZenitIrOperand - A symbol operand
 *
 */
static struct ZenitIrOperand visit_identifier(struct ZenitContext *ctx, struct ZenitNode *node, struct ZenitIrProgram *program)
{
    struct ZenitIdentifierNode *zenit_id = (struct ZenitIdentifierNode*)node;

    return (struct ZenitIrOperand) {
        .type = ZENIT_IR_OPERAND_SYMBOL,
        .operand.symbol = zenit_ir_symtable_get(&program->current->symtable, zenit_id->name)
    };
}

/*
 * Function: visit_array_initializer
 *  This function processes the array initializer to represent it as a value operand
 *  of an instruction
 *
 * Parameters:
 *  ctx - Context object
 *  node - Array initializer node
 *  program - Program object
 *
 * Returns:
 *  struct ZenitIrOperand - An array value operand
 *
 */
static struct ZenitIrOperand visit_array_initializer(struct ZenitContext *ctx, struct ZenitNode *node, struct ZenitIrProgram *program)
{
    struct ZenitArrayNode *array = (struct ZenitArrayNode*)node;

    struct ZenitIrArrayValue *arrayval = (struct ZenitIrArrayValue*)zenit_ir_value_new(ZENIT_IR_VALUE_ARRAY);
    zenit_type_to_zenit_ir_type(&array->base.typeinfo, &arrayval->base.typeinfo);

    arrayval->elements = fl_array_new(sizeof(struct ZenitIrOperand), fl_array_length(array->elements));

    for (size_t i=0; i < fl_array_length(array->elements); i++)
        arrayval->elements[i] = visit_node(ctx, array->elements[i], program);

    return (struct ZenitIrOperand) {
        .type = ZENIT_IR_OPERAND_VALUE,
        .operand.value = (struct ZenitIrValue*)arrayval
    };
}


/*
 * Function: visit_variable
 *  This function creates a new instruction that declares a named variable in the current
 *  block scope.
 *
 * Parameters:
 *  ctx - Context object
 *  node - Variable declaration node
 *  program - Program object
 *
 * Returns:
 *  struct ZenitIrOperand - The symbol operand that identifies the variable
 *
 */
static struct ZenitIrOperand visit_variable(struct ZenitContext *ctx, struct ZenitNode *node, struct ZenitIrProgram *program)
{
    // Get the <struct ZenitSymbol> object
    struct ZenitVariableNode *vardecl = (struct ZenitVariableNode*)node;
    struct ZenitSymbol *symbol = zenit_program_get_symbol(ctx->program, vardecl->name);
    
    // Create the variable declaration instruction and fill its operands
    struct ZenitIrVariableInstruction *varinstr = zenit_ir_instruction_variable_new();
    varinstr->attributes = zenit_attr_to_zenit_ir_attr(ctx, program, vardecl->attributes);

    // The lvalue is a symbol operand we create from the <struct ZenitSymbol>
    varinstr->lvalue = (struct ZenitIrOperand) {
        .type = ZENIT_IR_OPERAND_SYMBOL,
        .operand.symbol = zenit_ir_symtable_get(&program->current->symtable, vardecl->name)
    };

    // The rvalue is the operand we get from the visit to the <struct ZenitVariableNode>'s 
    // value
    varinstr->rvalue = visit_node(ctx, vardecl->value, program);

    // We add the instruction to the program
    zenit_ir_program_add_instruction(program, (struct ZenitIrInstruction*)varinstr);

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
 *  struct ZenitIrOperand - A pointer to a an operand object
 *
 */
static struct ZenitIrOperand visit_node(struct ZenitContext *ctx, struct ZenitNode *node, struct ZenitIrProgram *program)
{
    return generators[node->type](ctx, node, program);
}

static void import_zenit_ir_symbols_from_zenit_symbols(struct ZenitContext *ctx, struct ZenitIrProgram *program)
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
 *  We just iterate over the declarations visiting each node to populate the <struct ZenitIrProgram>
 *  with <struct ZenitIrInstruction>s
 */
struct ZenitIrProgram* zenit_generate_zir(struct ZenitContext *ctx)
{
    if (!ctx || !ctx->ast || !ctx->ast->decls)
        return NULL;

    struct ZenitIrProgram *program = zenit_ir_program_new();
    import_zenit_ir_symbols_from_zenit_symbols(ctx, program);

    size_t errors = zenit_context_error_count(ctx);

    for (size_t i=0; i < fl_array_length(ctx->ast->decls); i++)
        visit_node(ctx, ctx->ast->decls[i], program);

    if (errors == zenit_context_error_count(ctx))
        return program;

    zenit_ir_program_free(program);
    return NULL;
}
