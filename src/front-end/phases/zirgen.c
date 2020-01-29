#include "zirgen.h"
#include "utils.h"
#include "../program.h"
#include "../symbol.h"
#include "../../zir/program.h"
#include "../../zir/instructions/operands/operand.h"
#include "../../zir/instructions/operands/array.h"
#include "../../zir/instructions/operands/uint.h"
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

static inline struct ZirType* new_zir_type_from_zenit_type(struct ZirProgram *program, struct ZenitType *zenit_type);
static struct ZirAttributeMap zenit_attr_map_to_zir_attr_map(struct ZenitContext *ctx, struct ZirProgram *program, struct ZenitAttributeNodeMap zenit_attrs);

typedef struct ZirOperand*(*ZirGenerator)(struct ZenitContext *ctx, struct ZirProgram *program, struct ZenitNode *node);

// Visitor functions
static struct ZirOperand* visit_node(struct ZenitContext *ctx, struct ZirProgram *program, struct ZenitNode *node);
static struct ZirOperand* visit_uint_node(struct ZenitContext *ctx, struct ZirProgram *program, struct ZenitUintNode *primitive);
static struct ZirOperand* visit_variable_node(struct ZenitContext *ctx, struct ZirProgram *program, struct ZenitVariableNode *vardecl);
static struct ZirOperand* visit_array_node(struct ZenitContext *ctx, struct ZirProgram *program, struct ZenitArrayNode *array);
static struct ZirOperand* visit_identifier_node(struct ZenitContext *ctx, struct ZirProgram *program, struct ZenitIdentifierNode *id_node);
static struct ZirOperand* visit_reference_node(struct ZenitContext *ctx, struct ZirProgram *program, struct ZenitReferenceNode *ref_node);
static struct ZirOperand* visit_cast_node(struct ZenitContext *ctx, struct ZirProgram *program, struct ZenitCastNode *cast_node);
static struct ZirOperand* visit_field_decl_node(struct ZenitContext *ctx, struct ZirProgram *program, struct ZenitFieldDeclNode *field_node);
static struct ZirOperand* visit_struct_decl_node(struct ZenitContext *ctx, struct ZirProgram *program, struct ZenitStructDeclNode *struct_node);

/*
 * Variable: generators
 *  An array indexed with a <enum ZenitNodeKind> to get a <ZirGenerator> function
 */
static const ZirGenerator generators[] = {
    [ZENIT_NODE_UINT]           = (ZirGenerator) &visit_uint_node,
    [ZENIT_NODE_VARIABLE]       = (ZirGenerator) &visit_variable_node,
    [ZENIT_NODE_ARRAY]          = (ZirGenerator) &visit_array_node,
    [ZENIT_NODE_IDENTIFIER]     = (ZirGenerator) &visit_identifier_node,
    [ZENIT_NODE_REFERENCE]      = (ZirGenerator) &visit_reference_node,
    [ZENIT_NODE_CAST]           = (ZirGenerator) &visit_cast_node,
    [ZENIT_NODE_FIELD_DECL]     = (ZirGenerator) &visit_field_decl_node,
    [ZENIT_NODE_STRUCT_DECL]    = (ZirGenerator) &visit_struct_decl_node,
};

/*
 * Function: new_temp_symbol
 *  Creates a new temporal ZIR symbol and adds it to the current program's block
 *
 * Parameters:
 *  <struct ZirProgram> *program: The ZIR program
 *  <struct ZirType> *type: The type of the temporal symbol
 *
 * Returns:
 *  struct ZirSymbol*: The new temporal symbol
 *
 * Notes:
 *  Temporal symbols are symbols which name starts with a '%'
 */
static struct ZirSymbol* new_temp_symbol(struct ZirProgram *program, struct ZirType *type)
{
    char name[1024] = { 0 };
    snprintf(name, 1024, "%%tmp%llu", program->current->temp_counter++);

    struct ZirSymbol *zir_symbol = zir_symbol_new(name, type);

    zir_program_add_symbol(program, zir_symbol);

    return zir_symbol;
}

/*
 * Function: new_zir_type_from_zenit_type
 *  Converts a Zenit type object to its counterpart's ZIR type 
 *
 * Parameters:
 *  <struct ZirProgram> *program: ZIR program
 *  <struct ZenitType> *zenit_type: The Zenit type object to convert
 *
 * Returns:
 *  struct ZirType*: The ZIR type object
 */
static inline struct ZirType* new_zir_type_from_zenit_type(struct ZirProgram *program, struct ZenitType *zenit_type)
{
    if (zenit_type->typekind == ZENIT_TYPE_UINT)
    {
        struct ZenitUintType *zenit_uint = (struct ZenitUintType*) zenit_type;

        enum ZirUintTypeSize size;
        switch (zenit_uint->size)
        {
            case ZENIT_UINT_8: 
                size = ZIR_UINT_8; 
                break;
            
            case ZENIT_UINT_16:
                size = ZIR_UINT_16;
                break;

            default:
                size = ZIR_UINT_UNK;
                break;
        }

        return (struct ZirType*) zir_type_uint_new(size);
    }

    if (zenit_type->typekind == ZENIT_TYPE_REFERENCE)
    {
        struct ZenitReferenceType *zenit_ref = (struct ZenitReferenceType*) zenit_type;
        struct ZirType *zir_element_type = new_zir_type_from_zenit_type(program, zenit_ref->element);
        return (struct ZirType*) zir_type_reference_new(zir_element_type);
    }

    if (zenit_type->typekind == ZENIT_TYPE_STRUCT)
    {
        struct ZenitStructType *zenit_struct = (struct ZenitStructType*) zenit_type;
        return (struct ZirType*) zir_type_struct_new(zenit_struct->name);
    }

    if (zenit_type->typekind == ZENIT_TYPE_ARRAY)
    {
        struct ZenitArrayType *zenit_array = (struct ZenitArrayType*) zenit_type;
        
        struct ZirArrayType *zir_array = zir_type_array_new(new_zir_type_from_zenit_type(program, zenit_array->member_type));
        zir_array->length = zenit_array->length;
        
        return (struct ZirType*) zir_array;
    }

    if (zenit_type->typekind == ZENIT_TYPE_NONE)
        return zir_type_none_new();

    return NULL;
}

/*
 * Function: zenit_attr_map_to_zir_attr_map
 *  Converts a map of Zenit attributes to a map of ZIR attributes
 *
 * Parameters:
 *  <struct ZenitContext>  *ctx: Context object
 *  <struct ZirProgram>  *program: The ZIR program
 *  <struct ZenitAttributeNodeMap>  zenit_attrs: The Zenit attribute map
 *
 * Returns:
 *  struct ZirAttributeMap: The map of ZIR attributes converted from the Zenit attribute map
 */
static struct ZirAttributeMap zenit_attr_map_to_zir_attr_map(struct ZenitContext *ctx, struct ZirProgram *program, struct ZenitAttributeNodeMap zenit_attrs)
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

/*
 * Function: visit_cast_node
 *  Adds a cast instruction to the program using a temporal ZIR symbol to hold the intermediate
 *  result of the cast expression
 *
 * Parameters:
 *  <struct ZenitContext> *ctx: Context object
 *  <struct ZirProgram> *program: Program object
 *  <struct ZenitCastNode> *zenit_cast - The cast node
 *
 * Returns:
 *  struct ZirOperand - The cast operand object
 *
 */
static struct ZirOperand* visit_cast_node(struct ZenitContext *ctx, struct ZirProgram *program, struct ZenitCastNode *zenit_cast)
{
    // If it is an implicit cast (up cast), we let the back end manage it, so we directly return the operand
    // that comes up from the casted expression
    if (zenit_cast->implicit)
        return visit_node(ctx, program, zenit_cast->expression);

    struct ZirOperand *source = visit_node(ctx, program, zenit_cast->expression);

    struct ZenitSymbol *zenit_cast_symbol = zenit_utils_get_tmp_symbol(ctx->program, (struct ZenitNode*) zenit_cast);
    
    // We use a temporal symbol for the cast's destination. We copy the type informaton from the Zenit cast's object type information
    struct ZirSymbol *temp_symbol = new_temp_symbol(program, new_zir_type_from_zenit_type(program, zenit_cast_symbol->type));

    // Now, we need to get the source operand of the cast, and for that we need to visit the casted expression
    struct ZirOperand *destination = (struct ZirOperand*) zir_operand_symbol_new(temp_symbol);

    // We create the cast instruction with the temporal symbol as the destination operand
    struct ZirCastInstruction *cast_instr = zir_instruction_cast_new(destination, source);


    // We add the instruction to the program, and we return the destination operand
    return zir_program_emit(program, (struct ZirInstruction*) cast_instr)->destination;
}

/*
 * Function: visit_uint_node
 *  Adds a load instruction using the uint literal as the operand
 *
 * Parameters:
 *  <struct ZenitContext> *ctx: Context object
 *  <struct ZirProgram> *program: Program object
 *  <struct ZenitUintNode> *zenit_uint - Uint literal node
 *
 * Returns:
 *  struct ZirOperand - The uint operand object
 *
 */
static struct ZirOperand* visit_uint_node(struct ZenitContext *ctx, struct ZirProgram *program, struct ZenitUintNode *zenit_uint)
{
    struct ZenitSymbol *zenit_uint_symbol = zenit_utils_get_tmp_symbol(ctx->program, (struct ZenitNode*) zenit_uint);

    // First, we need to map the types and values between Zenit and ZIR
    union ZirUintValue zir_value;

    switch (((struct ZenitUintType*) zenit_uint_symbol->type)->size)
    {
        case ZENIT_UINT_8:
            zir_value.uint8 = zenit_uint->value.uint8;
            break;
        case ZENIT_UINT_16:
            zir_value.uint16 = zenit_uint->value.uint16;
            break;

        default:
            return NULL;
    }

    // Then, we create a primitive operand, and we copy the type information from the Zenit node
    struct ZirUintOperand *zir_uint_source = zir_operand_uint_new((struct ZirUintType*) new_zir_type_from_zenit_type(program, zenit_uint_symbol->type), zir_value);

    return (struct ZirOperand*) zir_uint_source;
}

/*
 * Function: visit_reference_node
 *  Adds a load instruction to the program using the referenced expression as the operand of the instruction
 *
 * Parameters:
 *  <struct ZenitContext> *ctx: Context object
 *  <struct ZirProgram> *program: Program object
 *  <struct ZenitReferenceNode> *zenit_ref - The reference node
 *
 * Returns:
 *  struct ZirOperand - The reference operand object
 *
 */
static struct ZirOperand* visit_reference_node(struct ZenitContext *ctx, struct ZirProgram *program, struct ZenitReferenceNode *zenit_ref)
{
    // We need to visit the referenced expression to get the operand
    struct ZirOperand *operand = visit_node(ctx, program, zenit_ref->expression);

    if (operand->type != ZIR_OPERAND_SYMBOL)
    {
        zenit_context_error(ctx, zenit_ref->base.location, ZENIT_ERROR_INVALID_REFERENCE, "Invalid usage of the reference operator");
        return NULL;
    }

    struct ZenitSymbol *zenit_ref_symbol = zenit_utils_get_tmp_symbol(ctx->program, (struct ZenitNode*) zenit_ref);

    // We use the load instruction
    struct ZirSymbol *temp_symbol = new_temp_symbol(program, new_zir_type_from_zenit_type(program, zenit_ref_symbol->type));
    struct ZirOperand *destination = (struct ZirOperand*) zir_operand_symbol_new(temp_symbol);    

    struct ZirReferenceType *ref_zir_type = (struct ZirReferenceType*) new_zir_type_from_zenit_type(program, zenit_ref_symbol->type);
    struct ZirReferenceOperand *ref_operand = zir_operand_reference_new(ref_zir_type, (struct ZirSymbolOperand*) operand);

    struct ZirVariableInstruction *var_instr = zir_instruction_variable_new(destination, (struct ZirOperand*) ref_operand);
    var_instr->attributes = zir_attribute_map_new();

    // We add the instruction and we return the destination operand
    return zir_program_emit(program, (struct ZirInstruction*) var_instr)->destination;
}

/*
 * Function: visit_identifier_node
 *  Adds a load instruction to the program using the identifier's symbol as the operand of the instruction
 *
 * Parameters:
 *  <struct ZenitContext> *ctx: Context object
 *  <struct ZirProgram> *program: Program object
 *  <struct ZenitIdentifierNode> *zenit_id - Literal node
 *
 * Returns:
 *  struct ZirOperand - The symbol operand object
 *
 */
static struct ZirOperand* visit_identifier_node(struct ZenitContext *ctx, struct ZirProgram *program, struct ZenitIdentifierNode *zenit_id)
{
    struct ZenitSymbol *zenit_symbol = zenit_program_get_symbol(ctx->program, zenit_id->name);

    // We retrieve the symbol from the symbol table
    struct ZirSymbol *zir_symbol = zir_symtable_get(&program->current->symtable, zenit_id->name);

    assert_or_return(ctx, zir_symbol != NULL, zenit_id->base.location, "ZIR symbol does not exist");

    return (struct ZirOperand*) zir_operand_symbol_new(zir_symbol);
}

/*
 * Function: visit_array_node
 *  Creates an array operand and adds a load instruction to the program
 *
 * Parameters:
 *  <struct ZenitContext> *ctx: Context object
 *  <struct ZirProgram> *program: Program object
 *  <struct ZenitArrayNode> *zenit_array - Array initializer node
 *
 * Returns:
 *  struct ZirOperand - They array operand
 *
 */
static struct ZirOperand* visit_array_node(struct ZenitContext *ctx, struct ZirProgram *program, struct ZenitArrayNode *zenit_array)
{
    struct ZenitSymbol *zenit_array_symbol = zenit_utils_get_tmp_symbol(ctx->program, (struct ZenitNode*) zenit_array);

    struct ZirArrayOperand *zir_array = zir_operand_array_new((struct ZirArrayType*) new_zir_type_from_zenit_type(program, zenit_array_symbol->type));

    // Visit the array's elements to get the operands
    for (size_t i=0; i < fl_array_length(zenit_array->elements); i++)
    {
        struct ZirOperand *zir_operand = visit_node(ctx, program, zenit_array->elements[i]);
        zir_operand_array_add_member(zir_array, zir_operand);
    }

    struct ZirSymbol *temp_symbol = new_temp_symbol(program, new_zir_type_from_zenit_type(program, zenit_array_symbol->type));
    struct ZirOperand *destination = (struct ZirOperand*) zir_operand_symbol_new(temp_symbol);

    // We use load instruction with array literals
    struct ZirVariableInstruction *var_instr = zir_instruction_variable_new(destination, (struct ZirOperand*) zir_array);
    var_instr->attributes = zir_attribute_map_new();

    // We add the instruction and we return the destination operand
    return zir_program_emit(program, (struct ZirInstruction*) var_instr)->destination;
}

/*
 * Function: visit_field_decl_node
 *  The field declaration visitor adds the ZIR symbols to the current block
 *
 * Parameters:
 *  <struct ZenitContext> *ctx: Context object
 *  <struct ZirProgram> *program: ZIR program
 *  <struct ZenitFieldDeclNode> *zenit_field: The field declaration node
 *
 * Returns:
 *  struct ZirOperand*: This function returns <NULL> as the field declaration does not add an instruction to the program
 */
static struct ZirOperand* visit_field_decl_node(struct ZenitContext *ctx, struct ZirProgram *program, struct ZenitFieldDeclNode *zenit_field)
{
    struct ZenitSymbol *zenit_symbol = zenit_program_get_symbol(ctx->program, zenit_field->name);
    
    struct ZirSymbol *zir_symbol = zir_symbol_new(zenit_field->name, new_zir_type_from_zenit_type(program, zenit_symbol->type));
    
    zir_symbol = zir_program_add_symbol(program, zir_symbol);

    assert_or_return(ctx, zir_symbol != NULL, zenit_field->base.location, "Could not create ZIR symbol");

    return NULL;
}

/*
 * Function: visit_struct_decl_node
 *  This function simply checks if the <convert_zenit_scope_to_zir_block> function has added the ZIR block that identifies
 *  the struct declaration
 *
 * Parameters:
 *  <struct ZenitContext> *ctx: Context object
 *  <struct ZirProgram> *program: Program object
 *  <struct ZenitStructDeclNode> *struct_node - Struct declaration node
 *
 * Returns:
 *  struct ZirOperand - <NULL>, because the struct declaration does not add an instruction
 */
static struct ZirOperand* visit_struct_decl_node(struct ZenitContext *ctx, struct ZirProgram *program, struct ZenitStructDeclNode *struct_node)
{
    struct ZirBlock *struct_block = zir_program_get_block(program, ZIR_BLOCK_STRUCT, struct_node->name);

    if (struct_block == NULL)
    {
        zenit_context_error(ctx, struct_node->base.location, ZENIT_ERROR_INTERNAL, "Missing ZIR block for struct '%s'", struct_node->name);
        return NULL;
    }

    zenit_program_push_scope(ctx->program, ZENIT_SCOPE_STRUCT, struct_node->name);
    zir_program_enter_block(program, struct_block);

    for (size_t i=0; i < fl_array_length(struct_node->members); i++)
        visit_node(ctx, program, struct_node->members[i]);

    zir_program_pop_block(program);
    zenit_program_pop_scope(ctx->program);

    return NULL;
}

/*
 * Function: visit_variable_node
 *  This function creates a new instruction that declares a named variable in the current
 *  block scope.
 *
 * Parameters:
 *  <struct ZenitContext> *ctx: Context object
 *  <struct ZirProgram> *program: Program object
 *  <struct ZenitVariableNode> *zenit_variable - Variable declaration node
 *
 * Returns:
 *  struct ZirOperand - The symbol operand that identifies the variable
 */
static struct ZirOperand* visit_variable_node(struct ZenitContext *ctx, struct ZirProgram *program, struct ZenitVariableNode *zenit_variable)
{
    struct ZenitSymbol *zenit_symbol = zenit_program_get_symbol(ctx->program, zenit_variable->name);
    
    struct ZirSymbol *zir_symbol = zir_symbol_new(zenit_variable->name, new_zir_type_from_zenit_type(program, zenit_symbol->type));
    
    zir_symbol = zir_program_add_symbol(program, zir_symbol);

    assert_or_return(ctx, zir_symbol != NULL, zenit_variable->base.location, "Could not create ZIR symbol");

    // The source operand is the one we get from the visit to the <struct ZenitVariableNode>'s value
    struct ZirOperand *lhs = (struct ZirOperand*) zir_operand_symbol_new(zir_symbol);
    struct ZirOperand *rhs = visit_node(ctx, program, zenit_variable->rvalue);

    // Create the variable declaration instruction with the source and destination operands
    struct ZirVariableInstruction *zir_varinst = zir_instruction_variable_new(lhs, rhs);
    zir_varinst->attributes = zenit_attr_map_to_zir_attr_map(ctx, program, zenit_variable->attributes);

    // We add the variable definition instruction to the program and finally return the destination operand
    return zir_program_emit(program, (struct ZirInstruction*) zir_varinst)->destination;
}

/*
 * Function: visit_node
 *  This function selects the visitor function based on the node's type
 *  and calls the function.
 *
 * Parameters:
 *  <struct ZenitContext> *ctx: Context object
 *  <struct ZirProgram> *program: Program object
 *  <struct ZenitNode> *node - Node to visit
 *
 * Returns:
 *  struct ZirOperand - A pointer to a an operand object
 *
 */
static struct ZirOperand* visit_node(struct ZenitContext *ctx, struct ZirProgram *program, struct ZenitNode *node)
{
    return generators[node->nodekind](ctx, program, node);
}

/*
 * Function: convert_zenit_scope_to_zir_block
 *  For every child of the Zenit scope object this function creates a ZIR block and adds it to
 *  the <struct ZirBlock> object.
 *
 * Parameters:
 *  <struct ZenitScope> *scope: The Zenit scope object used as the root for the conversion
 *  <struct ZirBlock> *block: The ZIR root block that will hold all the new ZIR blocks
 *
 * Returns:
 *  void: This function does not return a value
 */
static void convert_zenit_scope_to_zir_block(struct ZenitScope *scope, struct ZirBlock *block)
{
    for (size_t i=0; i < fl_array_length(scope->children); i++)
    {
        struct ZenitScope *zenit_child = scope->children[i];

        enum ZirBlockType block_type;

        switch (zenit_child->type)
        {
            case ZENIT_SCOPE_STRUCT:
                block_type = ZIR_BLOCK_STRUCT;
                break;
            case ZENIT_SCOPE_FUNCTION:
                block_type = ZIR_BLOCK_FUNCTION;
                break;
            case ZENIT_SCOPE_GLOBAL:
                block_type = ZIR_BLOCK_GLOBAL;
                break;
        }

        struct ZirBlock *zir_child = zir_block_new(zenit_child->id, block_type, block);
        block->children = fl_array_append(block->children, &zir_child);

        convert_zenit_scope_to_zir_block(zenit_child, zir_child);
    }
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

    size_t errors = zenit_context_error_count(ctx);

    // We make sure all the functions, structs, etc are "declared" in ZIR
    convert_zenit_scope_to_zir_block(ctx->program->global_scope, program->global);

    for (size_t i=0; i < fl_array_length(ctx->ast->decls); i++)
        visit_node(ctx, program, ctx->ast->decls[i]);

    if (errors == zenit_context_error_count(ctx))
        return program;

    zir_program_free(program);

    return NULL;
}
