#include "infer.h"
#include "program.h"
#include "symbol.h"
#include "../middle-end/program.h"
#include "../middle-end/value.h"

/*
 * Type: ZenitZirGenerator
 *  An inferrer function takes a symbol from the <struct ZenitSymbolTable> and updates
 *  it -if needed- with type information the symbol could be missing.
 *  The function returns a <struct ZenitTypeInfo> with the type retrieved from the symbol
 *  or inferred from the context.
 */
typedef struct ZirOperand(*ZenitZirGenerator)(struct ZenitContext *ctx, struct ZenitNode *node, struct ZirProgram *program);

// Visitor functions
static struct ZirOperand visit_node(struct ZenitContext *ctx, struct ZenitNode *node, struct ZirProgram *program);
static struct ZirOperand visit_literal(struct ZenitContext *ctx, struct ZenitNode *node, struct ZirProgram *program);
static struct ZirOperand visit_variable(struct ZenitContext *ctx, struct ZenitNode *node, struct ZirProgram *program);
static struct ZirOperand visit_array_initializer(struct ZenitContext *ctx, struct ZenitNode *node, struct ZirProgram *program);
static struct ZirOperand visit_identifier(struct ZenitContext *ctx, struct ZenitNode *node, struct ZirProgram *program);
static struct ZirOperand visit_unary_ref(struct ZenitContext *ctx, struct ZenitNode *node, struct ZirProgram *program);


static void zenit_type_to_zir_type(struct ZenitTypeInfo *zenit_type, struct ZirTypeInfo *zir_type)
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

struct ZirAttribute** zenit_attr_to_zir_attr(struct ZenitContext *ctx, struct ZirProgram *program, struct ZenitAttributeNode **zenit_attrs)
{
    if (!zenit_attrs)
        return NULL;

    size_t attr_count = fl_array_length(zenit_attrs);
    struct ZirAttribute **zir_attrs = fl_array_new(sizeof(struct ZirAttribute*), attr_count);

    for (size_t i=0; i < attr_count; i++)
    {
        struct ZenitAttributeNode *zenit_attr = zenit_attrs[i];
        struct ZirAttribute *zir_attr = fl_malloc(sizeof(struct ZirAttribute));

        zir_attr->name = fl_cstring_dup(zenit_attr->name);
        
        if (zenit_attr->properties)
        {
            size_t prop_count = fl_array_length(zenit_attr->properties);
            zir_attr->properties = fl_array_new(sizeof(struct ZirAttributeProperty), prop_count);

            for (size_t j=0; j < prop_count; j++)
            {
                struct ZenitAttributePropertyNode *zenit_prop = zenit_attr->properties + j;
                struct ZirAttributeProperty *zir_prop = zir_attr->properties + j;

                zir_prop->name = fl_cstring_dup(zenit_prop->name);

                struct ZirOperand zir_operand = visit_node(ctx, zenit_prop->value, program);
                memcpy(&zir_prop->value, &zir_operand, sizeof(struct ZirOperand));
            }
        }

        zir_attrs[i] = zir_attr;
    }

    return zir_attrs;
}

static struct ZirOperand new_operand_symbol(struct ZenitContext *ctx, struct ZenitSymbol *symbol, struct ZirProgram *program, bool global_symbol)
{
    struct ZirTypeInfo zir_symbol_type = { 0 };
    zenit_type_to_zir_type(&symbol->typeinfo, &zir_symbol_type);
    
    struct ZirSymbol *zir_symbol = zir_symbol_new(symbol->name, &zir_symbol_type);

    if (global_symbol)
        zir_program_add_global_symbol(program, zir_symbol);
    else
        zir_program_add_symbol(program, zir_symbol);

    return (struct ZirOperand) {
        .type =  ZIR_OPERAND_SYMBOL,
        .operand.symbol = zir_symbol
    };
}

/*
 * Variable: generators
 *  An array indexed with a <enum ZenitNodeType> to get a <ZenitZirGenerator> function
 */
static const ZenitZirGenerator generators[] = {
    [ZENIT_NODE_LITERAL]    = &visit_literal,
    [ZENIT_NODE_VARIABLE]   = &visit_variable,
    [ZENIT_NODE_ARRAY_INIT] = &visit_array_initializer,
    [ZENIT_NODE_IDENTIFIER] = &visit_identifier,
    [ZENIT_NODE_UNARY_REF] =  &visit_unary_ref,
};

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
 *  struct ZirOperand - A constant value operand
 *
 */
static struct ZirOperand visit_literal(struct ZenitContext *ctx, struct ZenitNode *node, struct ZirProgram *program)
{
    struct ZenitLiteralNode *literal = (struct ZenitLiteralNode*)node;

    struct ZirLiteralValue *literalval = (struct ZirLiteralValue*)zir_value_new(ZIR_VALUE_LITERAL);
    zenit_type_to_zir_type(&literal->base.typeinfo, &literalval->base.typeinfo);

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

    return (struct ZirOperand) {
        .type = ZIR_OPERAND_VALUE,
        .operand.value = (struct ZirValue*)literalval
    };
}

static struct ZirOperand visit_unary_ref(struct ZenitContext *ctx, struct ZenitNode *node, struct ZirProgram *program)
{
    struct ZenitUnaryRefNode *ref_node = (struct ZenitUnaryRefNode*)node;
    struct ZirOperand operand = visit_node(ctx, ref_node->expression, program);
    operand.type = ZIR_OPERAND_REF;
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
 *  struct ZirOperand - A symbol operand
 *
 */
static struct ZirOperand visit_identifier(struct ZenitContext *ctx, struct ZenitNode *node, struct ZirProgram *program)
{
    struct ZenitIdentifierNode *zenit_id = (struct ZenitIdentifierNode*)node;

    return (struct ZirOperand) {
        .type = ZIR_OPERAND_SYMBOL,
        .operand.symbol = zir_symtable_get(&program->current->symtable, zenit_id->name)
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
 *  struct ZirOperand - An array value operand
 *
 */
static struct ZirOperand visit_array_initializer(struct ZenitContext *ctx, struct ZenitNode *node, struct ZirProgram *program)
{
    struct ZenitArrayInitNode *array = (struct ZenitArrayInitNode*)node;

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
 *  struct ZirOperand - The symbol operand that identifies the variable
 *
 */
static struct ZirOperand visit_variable(struct ZenitContext *ctx, struct ZenitNode *node, struct ZirProgram *program)
{
    // Get the <struct ZenitSymbol> object
    struct ZenitVariableNode *vardecl = (struct ZenitVariableNode*)node;
    struct ZenitSymbol *symbol = zenit_program_get_symbol(ctx->program, vardecl->name);
    
    // Create the variable declaration instruction and fill its operands
    struct ZirVariableInstruction *varinstr = (struct ZirVariableInstruction*)zir_instruction_new(ZIR_INSTR_VARDECL);
    varinstr->attributes = zenit_attr_to_zir_attr(ctx, program, vardecl->attributes);

    // The lvalue is a symbol operand we create from the <struct ZenitSymbol>
    varinstr->lvalue = (struct ZirOperand) {
        .type = ZIR_OPERAND_SYMBOL,
        .operand.symbol = zir_symtable_get(&program->current->symtable, vardecl->name)
    };

    // The rvalue is the operand we get from the visit to the <struct ZenitVariableNode>'s 
    // value
    varinstr->rvalue = visit_node(ctx, vardecl->value, program);

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
        return false;

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
