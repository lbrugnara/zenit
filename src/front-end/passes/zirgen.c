#include "zirgen.h"
#include "utils.h"
#include "../program.h"
#include "../symbol.h"
#include "../../zir/program.h"
#include "../../zir/instructions/operands/operand.h"
#include "../../zir/instructions/operands/array.h"
#include "../../zir/instructions/operands/primitive.h"
#include "../../zir/instructions/operands/reference.h"
#include "../../zir/instructions/operands/symbol.h"

#define assert_or_return(ctx, condition, location, message)                 \
        if (!(condition))                                                   \
        {                                                                   \
            if (message != NULL)                                            \
                zenit_context_error(ctx, location,                          \
                    ZENIT_ERROR_INTERNAL, message);                         \
            return NULL;                                                    \
        }

static void import_zir_symbol_from_zenit_symbol(struct ZenitContext *ctx, struct ZenitSymbol *symbol, struct ZirProgram *program, bool global_symbol);
static inline void copy_zenit_type_to_zir_type(struct ZenitTypeInfo *zenit_type, struct ZirTypeInfo *zir_type);
static struct ZirAttributeMap zenit_attr_to_zir_attr(struct ZenitContext *ctx, struct ZirProgram *program, struct ZenitAttributeNodeMap zenit_attrs);

/*
 * Type: ZirGenerator
 *  An inferrer function takes a symbol from the <struct ZenitSymtable> and updates
 *  it -if needed- with type information the symbol could be missing.
 *  The function returns a <struct ZenitTypeInfo> with the type retrieved from the symbol
 *  or inferred from the context.
 */
typedef struct ZirOperand*(*ZirGenerator)(struct ZenitContext *ctx, struct ZirProgram *program, struct ZenitNode *node);

// Visitor functions
static struct ZirOperand* visit_node(struct ZenitContext *ctx, struct ZirProgram *program, struct ZenitNode *node);
static struct ZirOperand* visit_primitive_node(struct ZenitContext *ctx, struct ZirProgram *program, struct ZenitPrimitiveNode *primitive);
static struct ZirOperand* visit_variable_node(struct ZenitContext *ctx, struct ZirProgram *program, struct ZenitVariableNode *vardecl);
static struct ZirOperand* visit_array_node(struct ZenitContext *ctx, struct ZirProgram *program, struct ZenitArrayNode *array);
static struct ZirOperand* visit_identifier_node(struct ZenitContext *ctx, struct ZirProgram *program, struct ZenitIdentifierNode *id_node);
static struct ZirOperand* visit_reference_node(struct ZenitContext *ctx, struct ZirProgram *program, struct ZenitReferenceNode *ref_node);
static struct ZirOperand* visit_cast_node(struct ZenitContext *ctx, struct ZirProgram *program, struct ZenitCastNode *cast_node);

/*
 * Variable: generators
 *  An array indexed with a <enum ZenitNodeType> to get a <ZirGenerator> function
 */
static const ZirGenerator generators[] = {
    [ZENIT_NODE_PRIMITIVE]    = (ZirGenerator) &visit_primitive_node,
    [ZENIT_NODE_VARIABLE]   = (ZirGenerator) &visit_variable_node,
    [ZENIT_NODE_ARRAY]      = (ZirGenerator) &visit_array_node,
    [ZENIT_NODE_IDENTIFIER] = (ZirGenerator) &visit_identifier_node,
    [ZENIT_NODE_REFERENCE]  = (ZirGenerator) &visit_reference_node,
    [ZENIT_NODE_CAST]       = (ZirGenerator) &visit_cast_node,
};

static void import_zir_symbol_from_zenit_symbol(struct ZenitContext *ctx, struct ZenitSymbol *symbol, struct ZirProgram *program, bool global_symbol)
{
    struct ZirSymbol *zir_symbol = zir_symbol_new(symbol->name, NULL, false);
    copy_zenit_type_to_zir_type(symbol->typeinfo, &zir_symbol->typeinfo);

    zir_program_add_symbol(program, zir_symbol);
}

static struct ZirSymbol* new_temp_symbol(struct ZirProgram *program, bool global_symbol)
{
    char name[1024] = { 0 };
    snprintf(name, 1024, "tmp%llu", program->current->temp_counter++);

    struct ZirSymbol *zir_symbol = zir_symbol_new(name, NULL, true);

    zir_program_add_symbol(program, zir_symbol);

    return zir_symbol;
}

static inline void copy_zenit_type_to_zir_type(struct ZenitTypeInfo *zenit_type, struct ZirTypeInfo *zir_type)
{
    switch (zenit_type->type)
    {
        case ZENIT_TYPE_UINT8:
            zir_type->type = ZIR_TYPE_UINT8;
            break;

        case ZENIT_TYPE_UINT16:
            zir_type->type = ZIR_TYPE_UINT16;
            break;

        case ZENIT_TYPE_STRUCT:
            zir_type->type = ZIR_TYPE_CUSTOM;
            break;

        default:
            zir_type->type = ZIR_TYPE_NONE;
            break;
    }

    zir_type->elements = 1;
    zir_type->is_array = false;
    zir_type->name = NULL;

    if (zenit_type->type == ZENIT_TYPE_ARRAY)
    {
        struct ZenitArrayTypeInfo *zenit_array_type = (struct ZenitArrayTypeInfo*) zenit_type;
        copy_zenit_type_to_zir_type(zenit_array_type->member_type, zir_type);
        zir_type->is_array = true;
        zir_type->elements = zenit_array_type->length;
    }
    else if (zenit_type->type == ZENIT_TYPE_STRUCT)
    {
        struct ZenitStructTypeInfo *zenit_struct_type = (struct ZenitStructTypeInfo*) zenit_type;
        zir_type->type = ZIR_TYPE_CUSTOM;
        zir_type->name = zenit_struct_type->name ? fl_cstring_dup(zenit_struct_type->name) : NULL;
    }
    else if (zenit_type->type == ZENIT_TYPE_REFERENCE)
    {
        struct ZenitReferenceTypeInfo *zenit_ref_type = (struct ZenitReferenceTypeInfo*) zenit_type;
        copy_zenit_type_to_zir_type(zenit_ref_type->element, zir_type);
        zir_type->is_ref = true;        
    }
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

            // Create the ZIR property with the operand obtained from visiting the property's value
            struct ZirProperty *zir_prop = zir_property_new(zenit_prop->name, visit_node(ctx, program, zenit_prop->value));

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

static struct ZirOperand* visit_cast_node(struct ZenitContext *ctx, struct ZirProgram *program, struct ZenitCastNode *zenit_cast)
{
    // If it is an implicit cast (up cast), we let the back end manage it, so we directly return the operand
    // that comes up from the casted expression
    if (zenit_cast->implicit)
        return visit_node(ctx, program, zenit_cast->expression);

    struct ZenitSymbol *zenit_cast_symbol = zenit_utils_get_tmp_symbol(ctx->program, (struct ZenitNode*) zenit_cast);
    
    // We use a temporal symbol for the cast's destination. We copy the type informaton from the Zenit cast's object type information
    struct ZirSymbol *temp_symbol = new_temp_symbol(program, false);
    copy_zenit_type_to_zir_type(zenit_cast_symbol->typeinfo, &temp_symbol->typeinfo);

    // We create the cast instruction with the temporal symbol as the destination operand
    struct ZirCastInstruction *cast_instr = zir_instruction_cast_new((struct ZirOperand*) zir_operand_symbol_new(temp_symbol));

    // Now, we need to get the source operand of the cast, and for that we need to visit the casted expression
    cast_instr->source = visit_node(ctx, program, zenit_cast->expression);

    // We add the instruction to the program, and we return the destination operand
    return zir_program_emit(program, (struct ZirInstruction*) cast_instr)->destination;
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
static struct ZirOperand* visit_primitive_node(struct ZenitContext *ctx, struct ZirProgram *program, struct ZenitPrimitiveNode *zenit_primitive)
{
    struct ZenitSymbol *zenit_primitive_symbol = zenit_utils_get_tmp_symbol(ctx->program, (struct ZenitNode*) zenit_primitive);

    // First, we need to map the types and values between Zenit and ZIR
    enum ZirType zir_type = ZIR_TYPE_NONE;
    union ZirPrimitiveValue zir_value;

    switch (zenit_primitive_symbol->typeinfo->type)
    {
        case ZENIT_TYPE_UINT8:
            zir_type = ZIR_TYPE_UINT8;
            zir_value.uint8 = zenit_primitive->value.uint8;
            break;
        case ZENIT_TYPE_UINT16:
            zir_type = ZIR_TYPE_UINT16;
            zir_value.uint16 = zenit_primitive->value.uint16;
            break;

        default:
            return NULL;
    }

    // Then, we create a primitive operand, and we copy the type information from the Zenit node
    struct ZirPrimitiveOperand *zir_primitive = zir_operand_primitive_new(zir_type, zir_value);
    copy_zenit_type_to_zir_type(zenit_primitive_symbol->typeinfo, &zir_primitive->base.typeinfo);;

    // Now, we can create the load instruction
    struct ZirLoadInstruction *load_instr = zir_instruction_load_new((struct ZirOperand*) zir_primitive);

    // We add the instruction and we return the destination operand
    return zir_program_emit(program, (struct ZirInstruction*) load_instr)->destination;
}

static struct ZirOperand* visit_reference_node(struct ZenitContext *ctx, struct ZirProgram *program, struct ZenitReferenceNode *zenit_ref)
{
    // We need to visit the referenced expression to get the operand
    struct ZirOperand *operand = visit_node(ctx, program, zenit_ref->expression);

    if (operand->type != ZIR_OPERAND_SYMBOL)
    {
        zenit_context_error(ctx, zenit_ref->base.location, ZENIT_ERROR_INVALID_REFERENCE, "Invalid usage of the reference operator");
        return NULL;
    }

    // We use the load instruction
    struct ZirLoadInstruction *load_instr = zir_instruction_load_new((struct ZirOperand*) zir_operand_reference_new((struct ZirSymbolOperand*) operand));

    // We add the instruction and we return the destination operand
    return zir_program_emit(program, (struct ZirInstruction*) load_instr)->destination;
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
static struct ZirOperand* visit_identifier_node(struct ZenitContext *ctx, struct ZirProgram *program, struct ZenitIdentifierNode *zenit_id)
{
    // We retrieve the symbol from the symbol table
    struct ZirSymbol *zir_symbol = zir_symtable_get(&program->current->symtable, zenit_id->name);

    assert_or_return(ctx, zir_symbol != NULL, zenit_id->base.location, "ZIR symbol does not exist");

    // We use a load instruction for the symbol operand
    struct ZirLoadInstruction *load_instr = zir_instruction_load_new((struct ZirOperand*) zir_operand_symbol_new(zir_symbol));

    // We add the instruction and we return the dest operand
    return zir_program_emit(program, (struct ZirInstruction*) load_instr)->destination;
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
static struct ZirOperand* visit_array_node(struct ZenitContext *ctx, struct ZirProgram *program, struct ZenitArrayNode *zenit_array)
{
    struct ZirArrayOperand *zir_array = zir_operand_array_new();

    struct ZenitSymbol *zenit_array_symbol = zenit_utils_get_tmp_symbol(ctx->program, (struct ZenitNode*) zenit_array);

    // Copy the type information from the Zenit type information object
    copy_zenit_type_to_zir_type(zenit_array_symbol->typeinfo, &zir_array->base.typeinfo);

    // Visit the array's elements to get the operands
    for (size_t i=0; i < fl_array_length(zenit_array->elements); i++)
    {
        struct ZirOperand *zir_operand = visit_node(ctx, program, zenit_array->elements[i]);
        zir_array->elements = fl_array_append(zir_array->elements, &zir_operand);
    }

    // We use load instruction with array literals
    struct ZirLoadInstruction *load_instr = zir_instruction_load_new((struct ZirOperand*) zir_array);

    // We add the instruction and we return the destination operand
    return zir_program_emit(program, (struct ZirInstruction*) load_instr)->destination;
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
static struct ZirOperand* visit_variable_node(struct ZenitContext *ctx, struct ZirProgram *program, struct ZenitVariableNode *zenit_variable)
{
    // The destination of a variable definition instruction is a symbol, so we need the
    // symbol information from the symtable in order to create the destination operand
    struct ZirSymbol *zir_symbol = zir_symtable_get(&program->current->symtable, zenit_variable->name);

    assert_or_return(ctx, zir_symbol != NULL, zenit_variable->base.location, "Missing ZIR symbol");

    // Create the variable declaration instruction with the destination symbol operand
    struct ZirVariableInstruction *zir_varinst = zir_instruction_variable_new((struct ZirOperand*) zir_operand_symbol_new(zir_symbol));
    zir_varinst->attributes = zenit_attr_to_zir_attr(ctx, program, zenit_variable->attributes);

    // The source operand is the one we get from the visit to the <struct ZenitVariableNode>'s value
    zir_varinst->source = visit_node(ctx, program, zenit_variable->rvalue);

    // We add the variable definition instruction to the program and finally return the destination operand
    return zir_program_emit(program, (struct ZirInstruction*) zir_varinst)->destination;
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
static struct ZirOperand* visit_node(struct ZenitContext *ctx, struct ZirProgram *program, struct ZenitNode *node)
{
    return generators[node->type](ctx, program, node);
}

static void import_zir_symbols_from_zenit_symbols(struct ZenitContext *ctx, struct ZirProgram *program)
{
    // FIXME: Use a zenit_program function to get all the symbols
    // FIXME: Use the scopes!
    char **names = fl_hashtable_keys(ctx->program->global_scope->symtable.symbols);

    for (size_t i=0; i < fl_array_length(names); i++)
    {
        struct ZenitSymbol *zenit_symbol = zenit_program_get_symbol(ctx->program, names[i]);
        import_zir_symbol_from_zenit_symbol(ctx, zenit_symbol, program, ctx->program->global_scope->symtable.type == ZENIT_SYMTABLE_GLOBAL);
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
        visit_node(ctx, program, ctx->ast->decls[i]);

    if (errors == zenit_context_error_count(ctx))
        return program;

    zir_program_free(program);

    return NULL;
}
