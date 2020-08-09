#include "zirgen.h"
#include "utils.h"
#include "../program.h"
#include "../symbol.h"
#include "../../zir/program.h"
#include "../../zir/instructions/operands/pool.h"

#define assert_or_return(ctx, condition, location, message)                 \
        if (!(condition))                                                   \
        {                                                                   \
            if (message != NULL)                                            \
                zenit_context_error(ctx, location,                          \
                    ZENIT_ERROR_INTERNAL, message);                         \
            return NULL;                                                    \
        }

static inline ZirType* new_zir_type_from_zenit_type(ZirProgram *program, ZenitType *zenit_type);
ZirAttributeMap* zenit_attr_map_to_zir_attr_map(ZenitContext *ctx, ZirProgram *program, ZenitAttributeNodeMap *zenit_attrs);

typedef ZirOperand*(*ZirGenerator)(ZenitContext *ctx, ZirProgram *program, ZenitNode *node);

// Visitor functions
static ZirOperand* visit_node(ZenitContext *ctx, ZirProgram *program, ZenitNode *node);
static ZirOperand* visit_uint_node(ZenitContext *ctx, ZirProgram *program, ZenitUintNode *uint_node);
static ZirOperand* visit_bool_node(ZenitContext *ctx, ZirProgram *program, ZenitBoolNode *bool_node);
static ZirOperand* visit_variable_node(ZenitContext *ctx, ZirProgram *program, ZenitVariableNode *variable_node);
static ZirOperand* visit_array_node(ZenitContext *ctx, ZirProgram *program, ZenitArrayNode *array_node);
static ZirOperand* visit_identifier_node(ZenitContext *ctx, ZirProgram *program, ZenitIdentifierNode *id_node);
static ZirOperand* visit_reference_node(ZenitContext *ctx, ZirProgram *program, ZenitReferenceNode *ref_node);
static ZirOperand* visit_cast_node(ZenitContext *ctx, ZirProgram *program, ZenitCastNode *cast_node);
static ZirOperand* visit_field_decl_node(ZenitContext *ctx, ZirProgram *program, ZenitFieldDeclNode *field_node);
static ZirOperand* visit_struct_decl_node(ZenitContext *ctx, ZirProgram *program, ZenitStructDeclNode *struct_node);
static ZirOperand* visit_struct_node(ZenitContext *ctx, ZirProgram *program, ZenitStructNode *struct_node);

/*
 * Variable: generators
 *  An array indexed with a <ZenitNodeKind> to get a <ZirGenerator> function
 */
static const ZirGenerator generators[] = {
    [ZENIT_NODE_UINT]           = (ZirGenerator) &visit_uint_node,
    [ZENIT_NODE_BOOL]           = (ZirGenerator) &visit_bool_node,
    [ZENIT_NODE_VARIABLE]       = (ZirGenerator) &visit_variable_node,
    [ZENIT_NODE_ARRAY]          = (ZirGenerator) &visit_array_node,
    [ZENIT_NODE_IDENTIFIER]     = (ZirGenerator) &visit_identifier_node,
    [ZENIT_NODE_REFERENCE]      = (ZirGenerator) &visit_reference_node,
    [ZENIT_NODE_CAST]           = (ZirGenerator) &visit_cast_node,
    [ZENIT_NODE_FIELD_DECL]     = (ZirGenerator) &visit_field_decl_node,
    [ZENIT_NODE_STRUCT_DECL]    = (ZirGenerator) &visit_struct_decl_node,
    [ZENIT_NODE_STRUCT]         = (ZirGenerator) &visit_struct_node,
};

/*
 * Function: new_temp_symbol
 *  Creates a new temporal ZIR symbol and adds it to the current program's block
 *
 * Parameters:
 *  <ZirProgram> *program: The ZIR program
 *  <ZirType> *type: The type of the temporal symbol
 *
 * Returns:
 *  ZirSymbol*: The new temporal symbol
 *
 * Notes:
 *  Temporal symbols are symbols which name starts with a '%'
 */
static ZirSymbol* new_temp_symbol(ZirProgram *program, ZirType *type)
{
    char name[1024] = { 0 };
    snprintf(name, 1024, "%%tmp%llu", program->current->temp_counter++);

    ZirSymbol *zir_symbol = zir_symbol_new(name, type);

    zir_program_add_symbol(program, zir_symbol);

    return zir_symbol;
}

/*
 * Function: new_zir_type_from_zenit_type
 *  Converts a Zenit type object to its counterpart's ZIR type 
 *
 * Parameters:
 *  <ZirProgram> *program: ZIR program
 *  <ZenitType> *zenit_type: The Zenit type object to convert
 *
 * Returns:
 *  ZirType*: The ZIR type object
 */
static inline ZirType* new_zir_type_from_zenit_type(ZirProgram *program, ZenitType *zenit_type)
{
    if (zenit_type->typekind == ZENIT_TYPE_UINT)
    {
        ZenitUintType *zenit_uint = (ZenitUintType*) zenit_type;

        ZirUintTypeSize size;
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

        return (ZirType*) zir_type_uint_new(size);
    }

    if (zenit_type->typekind == ZENIT_TYPE_BOOL)
    {
        return (ZirType*) zir_type_bool_new();
    }

    if (zenit_type->typekind == ZENIT_TYPE_REFERENCE)
    {
        ZenitReferenceType *zenit_ref = (ZenitReferenceType*) zenit_type;
        ZirType *zir_element_type = new_zir_type_from_zenit_type(program, zenit_ref->element);
        return (ZirType*) zir_type_reference_new(zir_element_type);
    }

    if (zenit_type->typekind == ZENIT_TYPE_STRUCT)
    {
        ZenitStructType *zenit_struct = (ZenitStructType*) zenit_type;
        ZirStructType *zir_struct_type = zir_type_struct_new(zenit_struct->name);

        struct FlListNode *zenit_node = fl_list_head(zenit_struct->members);

        while (zenit_node)
        {
            ZenitStructTypeMember *zenit_member = (ZenitStructTypeMember*) zenit_node->value;
            zir_type_struct_add_member(zir_struct_type, zenit_member->name, new_zir_type_from_zenit_type(program, zenit_member->type));
            zenit_node = zenit_node->next;
        }

        return (ZirType*) zir_struct_type;
    }

    if (zenit_type->typekind == ZENIT_TYPE_ARRAY)
    {
        ZenitArrayType *zenit_array = (ZenitArrayType*) zenit_type;
        
        ZirArrayType *zir_array = zir_type_array_new(new_zir_type_from_zenit_type(program, zenit_array->member_type));
        zir_array->length = zenit_array->length;
        
        return (ZirType*) zir_array;
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
 *  <ZenitContext>  *ctx: Context object
 *  <ZirProgram>  *program: The ZIR program
 *  <ZenitAttributeNodeMap> *zenit_attrs: The Zenit attribute map
 *
 * Returns:
 *  ZirAttributeMap: The map of ZIR attributes converted from the Zenit attribute map
 */
ZirAttributeMap* zenit_attr_map_to_zir_attr_map(ZenitContext *ctx, ZirProgram *program, ZenitAttributeNodeMap *zenit_attrs)
{
    // We always initialize the ZIR attributes map
    ZirAttributeMap *zir_attrs = zir_attribute_map_new();

    // Get the Zenit attributes
    const char **zenit_attr_names = zenit_attribute_node_map_keys(zenit_attrs);
    size_t zenit_attr_count = fl_array_length(zenit_attr_names);    

    for (size_t i=0; i < zenit_attr_count; i++)
    {
        // Get the Zenit attribute
        ZenitAttributeNode *zenit_attr = zenit_attribute_node_map_get(zenit_attrs, zenit_attr_names[i]);
        // Get the Zenit properties
        const char **zenit_prop_names = zenit_property_node_map_keys(zenit_attr->properties);
        size_t zenit_prop_count = fl_array_length(zenit_prop_names);

        // Create the ZIR attribute
        ZirAttribute *zir_attr = zir_attribute_new(zenit_attr->name);    

        // Create the ZIR properties (if any)
        for (size_t j=0; j < zenit_prop_count; j++)
        {
            // Get the Zenit property
            ZenitPropertyNode *zenit_prop = zenit_property_node_map_get(zenit_attr->properties, zenit_prop_names[j]);

            // Create the ZIR property with the operand obtained from visiting the property's value
            ZirProperty *zir_prop = zir_property_new(zenit_prop->name, visit_node(ctx, program, zenit_prop->value));

            // We add the parsed property to the attribute's properties map
            zir_property_map_add(zir_attr->properties, zir_prop);
        }

        fl_array_free(zenit_prop_names);

        // Add the ZIR attribute to the map
        zir_attribute_map_add(zir_attrs, zir_attr);
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
 *  <ZenitContext> *ctx: Context object
 *  <ZirProgram> *program: Program object
 *  <ZenitCastNode> *zenit_cast - The cast node
 *
 * Returns:
 *  ZirOperand - The cast operand object
 *
 */
static ZirOperand* visit_cast_node(ZenitContext *ctx, ZirProgram *program, ZenitCastNode *zenit_cast)
{
    // If it is an implicit cast (up cast), we let the back end manage it, so we directly return the operand
    // that comes up from the casted expression
    if (zenit_cast->implicit)
        return visit_node(ctx, program, zenit_cast->expression);

    ZirOperand *source = visit_node(ctx, program, zenit_cast->expression);

    ZenitSymbol *zenit_cast_symbol = zenit_utils_get_tmp_symbol(ctx->program, (ZenitNode*) zenit_cast);
    
    // We use a temporal symbol for the cast's destination. We copy the type informaton from the Zenit cast's object type information
    ZirSymbol *temp_symbol = new_temp_symbol(program, new_zir_type_from_zenit_type(program, zenit_cast_symbol->type));

    // Now, we need to get the source operand of the cast, and for that we need to visit the casted expression
    ZirOperand *destination = (ZirOperand*) zir_operand_pool_new_symbol(program->operands, temp_symbol);

    // We create the cast instruction with the temporal symbol as the destination operand
    ZirCastInstruction *cast_instr = zir_instruction_cast_new(destination, source);

    // We add the instruction to the program, and we return the destination operand
    return zir_program_emit(program, (ZirInstruction*) cast_instr)->destination;
}

/*
 * Function: visit_uint_node
 *  Returns a uint operand
 *
 * Parameters:
 *  <ZenitContext> *ctx: Context object
 *  <ZirProgram> *program: Program object
 *  <ZenitUintNode> *zenit_uint - Uint literal node
 *
 * Returns:
 *  ZirOperand - The uint operand object
 *
 */
static ZirOperand* visit_uint_node(ZenitContext *ctx, ZirProgram *program, ZenitUintNode *zenit_uint)
{
    ZenitSymbol *zenit_uint_symbol = zenit_utils_get_tmp_symbol(ctx->program, (ZenitNode*) zenit_uint);

    // First, we need to map the types and values between Zenit and ZIR
    ZirUintValue zir_value;

    switch (((ZenitUintType*) zenit_uint_symbol->type)->size)
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

    // Then, we create a uint operand, and we copy the type information from the Zenit node
    ZirUintOperand *zir_uint_source = zir_operand_pool_new_uint(program->operands, (ZirUintType*) new_zir_type_from_zenit_type(program, zenit_uint_symbol->type), zir_value);

    return (ZirOperand*) zir_uint_source;
}

/*
 * Function: visit_bool_node
 *  Returns a boolean operand
 *
 * Parameters:
 *  <ZenitContext> *ctx: Context object
 *  <ZirProgram> *program: Program object
 *  <ZenitBoolNode> *zenit_bool - Boolean literal node
 *
 * Returns:
 *  ZirOperand - The boolean operand object
 *
 */
static ZirOperand* visit_bool_node(ZenitContext *ctx, ZirProgram *program, ZenitBoolNode *zenit_bool)
{
    ZenitSymbol *zenit_bool_symbol = zenit_utils_get_tmp_symbol(ctx->program, (ZenitNode*) zenit_bool);

    // Then, we create a boolean operand, and we copy the type information from the Zenit node
    ZirBoolType *bool_type = (ZirBoolType*) new_zir_type_from_zenit_type(program, zenit_bool_symbol->type);
    ZirBoolOperand *zir_bool_operand = zir_operand_pool_new_bool(program->operands, bool_type, zenit_bool->value);

    return (ZirOperand*) zir_bool_operand;
}

/*
 * Function: visit_reference_node
 *  Returns a reference operand for the referenced expression
 *
 * Parameters:
 *  <ZenitContext> *ctx: Context object
 *  <ZirProgram> *program: Program object
 *  <ZenitReferenceNode> *zenit_ref - The reference node
 *
 * Returns:
 *  ZirOperand - The reference operand object
 *
 */
static ZirOperand* visit_reference_node(ZenitContext *ctx, ZirProgram *program, ZenitReferenceNode *zenit_ref)
{
    // We need to visit the referenced expression to get the operand
    ZirOperand *operand = visit_node(ctx, program, zenit_ref->expression);

    if (operand->type != ZIR_OPERAND_SYMBOL)
    {
        zenit_context_error(ctx, zenit_ref->base.location, ZENIT_ERROR_INVALID_REFERENCE, "Invalid usage of the reference operator");
        return NULL;
    }
    
    ZenitSymbol *zenit_ref_symbol = zenit_utils_get_tmp_symbol(ctx->program, (ZenitNode*) zenit_ref);
    
    // We convert from the Zenit type to the ZIR type
    ZirReferenceType *ref_zir_type = (ZirReferenceType*) new_zir_type_from_zenit_type(program, zenit_ref_symbol->type);

    // We return a reference operand with the symbol operand we received from the visit to the referenced expression
    return (ZirOperand*) zir_operand_pool_new_reference(program->operands, ref_zir_type, (ZirSymbolOperand*) operand);    
}

/*
 * Function: visit_identifier_node
 *  Returns a symbol operand for the identifier
 *
 * Parameters:
 *  <ZenitContext> *ctx: Context object
 *  <ZirProgram> *program: Program object
 *  <ZenitIdentifierNode> *zenit_id - Literal node
 *
 * Returns:
 *  ZirOperand - The symbol operand object
 *
 */
static ZirOperand* visit_identifier_node(ZenitContext *ctx, ZirProgram *program, ZenitIdentifierNode *zenit_id)
{
    ZenitSymbol *zenit_symbol = zenit_program_get_symbol(ctx->program, zenit_id->name);

    // We retrieve the symbol from the symbol table
    ZirSymbol *zir_symbol = zir_symtable_get(&program->current->symtable, zenit_id->name);

    assert_or_return(ctx, zir_symbol != NULL, zenit_id->base.location, "ZIR symbol does not exist");

    return (ZirOperand*) zir_operand_pool_new_symbol(program->operands, zir_symbol);
}

/*
 * Function: visit_array_node
 *  Creates an array operand for the array literal
 *
 * Parameters:
 *  <ZenitContext> *ctx: Context object
 *  <ZirProgram> *program: Program object
 *  <ZenitArrayNode> *zenit_array - Array initializer node
 *
 * Returns:
 *  ZirOperand - They array operand
 *
 */
static ZirOperand* visit_array_node(ZenitContext *ctx, ZirProgram *program, ZenitArrayNode *zenit_array)
{
    ZenitSymbol *zenit_array_symbol = zenit_utils_get_tmp_symbol(ctx->program, (ZenitNode*) zenit_array);

    // We create an array operand for the array literal
    ZirArrayType *zir_array_type = (ZirArrayType*) new_zir_type_from_zenit_type(program, zenit_array_symbol->type);
    ZirArrayOperand *zir_array = zir_operand_pool_new_array(program->operands, zir_array_type);

    // Visit the array's elements to get the operands
    for (size_t i=0; i < fl_array_length(zenit_array->elements); i++)
    {
        ZirOperand *zir_operand = visit_node(ctx, program, zenit_array->elements[i]);
        zir_operand_array_add_element(zir_array, zir_operand);
    }

    return (ZirOperand*) zir_array;
}

static ZirOperand* visit_struct_node(ZenitContext *ctx, ZirProgram *program, ZenitStructNode *zenit_struct)
{
    ZenitSymbol *zenit_struct_symbol = zenit_utils_get_tmp_symbol(ctx->program, (ZenitNode*) zenit_struct);

    // Create struct operand
    ZirStructType *zir_struct_type = (ZirStructType*) new_zir_type_from_zenit_type(program, zenit_struct_symbol->type);
    ZirStructOperand *struct_operand = zir_operand_pool_new_struct(program->operands, zir_struct_type);

    for (size_t i=0; i < fl_array_length(zenit_struct->members); i++)
    {
        ZenitNode *member_node = zenit_struct->members[i];

        if (member_node->nodekind == ZENIT_NODE_FIELD)
        {
            ZenitFieldNode *field_node = (ZenitFieldNode*) member_node;
            ZirOperand *field_operand = visit_node(ctx, program, field_node->value);
            zir_operand_struct_add_member(struct_operand, field_node->name, field_operand);
        }
    }

    // Return struct operand
    return (ZirOperand*) struct_operand;
}

/*
 * Function: visit_field_decl_node
 *  The field declaration visitor adds the ZIR symbols to the current block
 *
 * Parameters:
 *  <ZenitContext> *ctx: Context object
 *  <ZirProgram> *program: ZIR program
 *  <ZenitFieldDeclNode> *zenit_field: The field declaration node
 *
 * Returns:
 *  ZirOperand*: This function returns <NULL> as the field declaration does not add an instruction to the program
 */
static ZirOperand* visit_field_decl_node(ZenitContext *ctx, ZirProgram *program, ZenitFieldDeclNode *zenit_field)
{
    ZenitSymbol *zenit_symbol = zenit_program_get_symbol(ctx->program, zenit_field->name);

    // We add a new symbol in the current block.   
    ZirSymbol *zir_symbol = zir_symbol_new(zenit_field->name, new_zir_type_from_zenit_type(program, zenit_symbol->type));    
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
 *  <ZenitContext> *ctx: Context object
 *  <ZirProgram> *program: Program object
 *  <ZenitStructDeclNode> *struct_node - Struct declaration node
 *
 * Returns:
 *  ZirOperand - <NULL>, because the struct declaration does not add an instruction
 */
static ZirOperand* visit_struct_decl_node(ZenitContext *ctx, ZirProgram *program, ZenitStructDeclNode *struct_node)
{
    ZirBlock *struct_block = zir_program_get_block(program, ZIR_BLOCK_STRUCT, struct_node->name);

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
 *  <ZenitContext> *ctx: Context object
 *  <ZirProgram> *program: Program object
 *  <ZenitVariableNode> *zenit_variable - Variable declaration node
 *
 * Returns:
 *  ZirOperand - The symbol operand that identifies the variable
 */
static ZirOperand* visit_variable_node(ZenitContext *ctx, ZirProgram *program, ZenitVariableNode *zenit_variable)
{
    ZenitSymbol *zenit_symbol = zenit_program_get_symbol(ctx->program, zenit_variable->name);
    
    ZirSymbol *zir_symbol = zir_symbol_new(zenit_variable->name, new_zir_type_from_zenit_type(program, zenit_symbol->type));
    
    zir_symbol = zir_program_add_symbol(program, zir_symbol);

    assert_or_return(ctx, zir_symbol != NULL, zenit_variable->base.location, "Could not create ZIR symbol");

    // The destination operand is a symbol operand
    ZirOperand *lhs = (ZirOperand*) zir_operand_pool_new_symbol(program->operands, zir_symbol);
    // The source operand is the one we get from the visit to the <ZenitVariableNode>'s value
    ZirOperand *rhs = visit_node(ctx, program, zenit_variable->rvalue);

    // Create the variable declaration instruction with the source and destination operands
    ZirVariableInstruction *zir_varinst = zir_instruction_variable_new(lhs, rhs);
    zir_varinst->attributes = zenit_attr_map_to_zir_attr_map(ctx, program, zenit_variable->attributes);

    // We add the variable definition instruction to the program and finally return the destination operand
    return zir_program_emit(program, (ZirInstruction*) zir_varinst)->destination;
}

/*
 * Function: visit_node
 *  This function selects the visitor function based on the node's type
 *  and calls the function.
 *
 * Parameters:
 *  <ZenitContext> *ctx: Context object
 *  <ZirProgram> *program: Program object
 *  <ZenitNode> *node - Node to visit
 *
 * Returns:
 *  ZirOperand - A pointer to a an operand object
 *
 */
static ZirOperand* visit_node(ZenitContext *ctx, ZirProgram *program, ZenitNode *node)
{
    return generators[node->nodekind](ctx, program, node);
}

/*
 * Function: convert_zenit_scope_to_zir_block
 *  For every child of the Zenit scope object this function creates a ZIR block and adds it to
 *  the <ZirBlock> object.
 *
 * Parameters:
 *  <ZenitScope> *scope: The Zenit scope object used as the root for the conversion
 *  <ZirBlock> *block: The ZIR root block that will hold all the new ZIR blocks
 *
 * Returns:
 *  void: This function does not return a value
 */
static void convert_zenit_scope_to_zir_block(ZenitScope *scope, ZirBlock *block)
{
    for (size_t i=0; i < fl_array_length(scope->children); i++)
    {
        ZenitScope *zenit_child = scope->children[i];

        ZirBlockType block_type;

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

        ZirBlock *zir_child = zir_block_new(zenit_child->id, block_type, block);
        block->children = fl_array_append(block->children, &zir_child);

        convert_zenit_scope_to_zir_block(zenit_child, zir_child);
    }
}

/*
 * Function: zenit_generate_zir
 *  We just iterate over the declarations visiting each node to populate the <ZirProgram>
 *  with <ZirInstruction>s
 */
ZirProgram* zenit_generate_zir(ZenitContext *ctx)
{
    if (!ctx || !ctx->ast || !ctx->ast->decls)
        return NULL;

    ZirProgram *program = zir_program_new();

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
