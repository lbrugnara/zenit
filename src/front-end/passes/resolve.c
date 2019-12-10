#include "resolve.h"
#include "utils.h"
#include "../program.h"
#include "../symbol.h"
#include "../types/system.h"

/*
 * Type: ZenitSymbolResolver
 *  A resolver function takes the <struct ZenitSymtable> and a <struct ZenitNode> and
 *  populates the table (if needed) with symbol information.
 *  The function returns a <struct ZenitSymbol> if the node updates the table or NULL
 *  if there is nothing to update
 */
typedef struct ZenitSymbol*(*ZenitSymbolResolver)(struct ZenitContext *ctx, struct ZenitNode *node);

// Visitor functions
static struct ZenitSymbol* visit_node(struct ZenitContext *ctx, struct ZenitNode *node);
static struct ZenitSymbol* visit_primitive_node(struct ZenitContext *ctx, struct ZenitPrimitiveNode *node);
static struct ZenitSymbol* visit_variable_node(struct ZenitContext *ctx, struct ZenitVariableNode *node);
static struct ZenitSymbol* visit_array_node(struct ZenitContext *ctx, struct ZenitArrayNode *node);
static struct ZenitSymbol* visit_identifier_node(struct ZenitContext *ctx, struct ZenitIdentifierNode *node);
static struct ZenitSymbol* visit_reference_node(struct ZenitContext *ctx, struct ZenitReferenceNode *node);
static struct ZenitSymbol* visit_cast_node(struct ZenitContext *ctx, struct ZenitCastNode *node);

/*
 * Variable: symbol_resolvers
 *  An array indexed with a <enum ZenitNodeType> to get a <ZenitSymbolResolver> function
 */
static const ZenitSymbolResolver symbol_resolvers[] = {
    [ZENIT_NODE_VARIABLE]   = (ZenitSymbolResolver) &visit_variable_node,
    [ZENIT_NODE_IDENTIFIER] = (ZenitSymbolResolver) &visit_identifier_node,
    [ZENIT_NODE_ARRAY]      = (ZenitSymbolResolver) &visit_array_node,
    [ZENIT_NODE_REFERENCE]  = (ZenitSymbolResolver) &visit_reference_node,
    [ZENIT_NODE_CAST]       = (ZenitSymbolResolver) &visit_cast_node,
    [ZENIT_NODE_PRIMITIVE]    = (ZenitSymbolResolver) &visit_primitive_node,
};

static struct ZenitTypeInfo* build_type_info_from_declaration(struct ZenitProgram *program, struct ZenitTypeNode *type_decl)
{
    struct ZenitTypeInfo *typeinfo = NULL;

    if (type_decl == NULL)
    {
        typeinfo = zenit_type_none_new();
    }
    else if (type_decl->base.type == ZENIT_NODE_TYPE_PRIMITIVE)
    {
        typeinfo = (struct ZenitTypeInfo*) zenit_type_primitive_new(type_decl->type);
    }
    else if (type_decl->base.type == ZENIT_NODE_TYPE_STRUCT)
    {
        // FIXME: Add members once they are implemented
        struct ZenitStructTypeNode *struct_type_decl = (struct ZenitStructTypeNode*) type_decl;
        typeinfo = (struct ZenitTypeInfo*) zenit_type_struct_new(struct_type_decl->name);
    }
    else if (type_decl->base.type == ZENIT_NODE_TYPE_REFERENCE)
    {
        struct ZenitReferenceTypeNode *ref_type_decl = (struct ZenitReferenceTypeNode*) type_decl;
        typeinfo = (struct ZenitTypeInfo*) zenit_type_reference_new(build_type_info_from_declaration(program, ref_type_decl->element));
    }
    else if (type_decl->base.type == ZENIT_NODE_TYPE_ARRAY)
    {
        struct ZenitArrayTypeNode *array_type_decl = (struct ZenitArrayTypeNode*) type_decl;
        struct ZenitArrayTypeInfo *array_type = zenit_type_array_new();
        array_type->source = ZENIT_ARRAY_TYPE_DECL;
        array_type->member_type = build_type_info_from_declaration(program, array_type_decl->members_type);
        array_type->length = array_type_decl->length;

        typeinfo = (struct ZenitTypeInfo*) array_type;
        
    }
    else
    {
        typeinfo = zenit_type_none_new();
    }

    // We track the type with the program object because the program will take care of freeing the memory
    // once it is freed too
    return zenit_type_pool_register(&program->type_pool, typeinfo);
}

/*
 * Function: visit_primitive_node
 *  Some leaves in the AST do not perform symbol-related actions so this
 *  function simply returns NULL
 *
 * Parameters:
 *  <struct ZenitContext> *ctx - Context object
 *  <struct ZenitNode> *node - Node object
 *
 * Returns:
 *  struct ZenitSymbol* - This function always returns NULL
 *
 */
static struct ZenitSymbol* visit_primitive_node(struct ZenitContext *ctx, struct ZenitPrimitiveNode *node)
{
    struct ZenitTypeInfo *typeinfo = zenit_type_pool_register(&ctx->program->type_pool, (struct ZenitTypeInfo*) zenit_type_primitive_new(node->type));

    return zenit_utils_new_tmp_symbol(ctx->program, (struct ZenitNode*) node, typeinfo);
}

/*
 * Function: visit_cast_node
 *  A cast expression does not perform changes to the symbol table, but we need to
 *  visit the casted expression node
 *
 * Parameters:
 *  <struct ZenitContext> *ctx - Context object
 *  <struct ZenitCastNode> *cast_node - Visited node
 *
 * Returns:
 *  struct ZenitSymbol* - This function always returns NULL
 *
 */
static struct ZenitSymbol* visit_cast_node(struct ZenitContext *ctx, struct ZenitCastNode *cast_node)
{
    struct ZenitSymbol *expr_symbol = visit_node(ctx, cast_node->expression);

    if (expr_symbol == NULL)
        return NULL;

    return zenit_utils_new_tmp_symbol(ctx->program, (struct ZenitNode*) cast_node, build_type_info_from_declaration(ctx->program, cast_node->type_decl));
}

/*
 * Function: visit_identifier_node
 *  An identifier must be registered in the symbol table to be valid, so this function
 *  performs that check, if the symbol does not exist it registers an error
 *
 * Parameters:
 *  <struct ZenitContext> *ctx - Context object
 *  <struct ZenitIdentifierNode> *id_node - Visited node
 *
 * Returns:
 *  struct ZenitSymbol* - A valid symbol if the identifier is valid, otherwise NULL
 *
 * Notes:
 *  FIXME: In the future we will need a "unresolved symbol" reference to avoid forward declarations
 */
static struct ZenitSymbol* visit_identifier_node(struct ZenitContext *ctx, struct ZenitIdentifierNode *id_node)
{
    // If the symbol exists we return it to the caller
    if (zenit_program_has_symbol(ctx->program, id_node->name))
        return zenit_program_get_symbol(ctx->program, id_node->name);

    // By now, if the identifier does not exist, we add an error (we are not declaring variables or types at the moment, so a missing identifier
    // is a not defined variable)
    zenit_context_error(ctx, id_node->base.location, ZENIT_ERROR_MISSING_SYMBOL, "Identifier %s is not defined.", id_node->name);

    return NULL;
}

/*
 * Function: visit_reference_node
 *  A reference expression does not perform changes to the symbol table, but we need to
 *  visit the referenced expression node
 *
 * Parameters:
 *  <struct ZenitContext> *ctx - Context object
 *  <struct ZenitReferenceNode> *reference_node - Visited node
 *
 * Returns:
 *  struct ZenitSymbol* - This function always returns NULL
 *
 */
static struct ZenitSymbol* visit_reference_node(struct ZenitContext *ctx, struct ZenitReferenceNode *reference_node)
{
    struct ZenitSymbol *expr_symbol = visit_node(ctx, reference_node->expression);

    if (expr_symbol == NULL)
        return NULL;

    struct ZenitTypeInfo *typeinfo = zenit_type_pool_register(&ctx->program->type_pool, (struct ZenitTypeInfo*) zenit_type_reference_new(expr_symbol->typeinfo));

    return zenit_utils_new_tmp_symbol(ctx->program, (struct ZenitNode*) reference_node, typeinfo);
}

/*
 * Function: visit_array_node
 *  An array does not perform changes to the symbol table, but we need to
 *  visit each node within the array
 *
 * Parameters:
 *  <struct ZenitContext> *ctx - Context object
 *  <struct ZenitArrayNode> *array_node - Visited node
 *
 * Returns:
 *  struct ZenitSymbol* - This function always returns NULL
 *
 */
static struct ZenitSymbol* visit_array_node(struct ZenitContext *ctx, struct ZenitArrayNode *array_node)
{
    struct ZenitArrayTypeInfo *array_type = zenit_type_pool_register_array(&ctx->program->type_pool, zenit_type_array_new());
    
    // The length is the number of elements within the array initializer
    array_type->length = fl_array_length(array_node->elements);

    // The member's type is NONE by now, the infer pass will take care of it
    array_type->member_type = zenit_type_pool_register(&ctx->program->type_pool, zenit_type_none_new());

    for (size_t i=0; i < fl_array_length(array_node->elements); i++)
    {
        struct ZenitSymbol *element_symbol = visit_node(ctx, array_node->elements[i]);

        struct ZenitTypeInfo *element_typeinfo = NULL;
        if (element_symbol == NULL)
        {
            // Something happened, we use a dummy type to continue with the compilation, but the "visit_node" 
            // call should have added an error
            element_typeinfo =  zenit_type_pool_register(&ctx->program->type_pool, (struct ZenitTypeInfo*) zenit_type_none_new());
        }
        else
        {
            element_typeinfo = element_symbol->typeinfo;
        }

        zenit_type_array_add_member(array_type, element_typeinfo);
    }

    return zenit_utils_new_tmp_symbol(ctx->program, (struct ZenitNode*) array_node, (struct ZenitTypeInfo*) array_type);
}

/*
 * Function: visit_attribute_node_map
 *  We iterate over all the attributes to make sure the properties' values are valid symbols
 *
 * Parameters:
 *  <struct ZenitContext> *ctx - Context object
 *  <struct ZenitAttributeNodeMap> *attributes: Attributes map
 * 
 * Returns:
 *  void - This function does not return a value
 *
 */
static void visit_attribute_node_map(struct ZenitContext *ctx, struct ZenitAttributeNodeMap *attributes)
{
    const char **names = zenit_attribute_node_map_keys(attributes);
    for (size_t i=0; i < fl_array_length(names); i++)
    {
        struct ZenitAttributeNode *attr = zenit_attribute_node_map_get(attributes, names[i]);

        struct ZenitPropertyNode **properties = zenit_property_node_map_values(&attr->properties);
        for (size_t j=0; j < fl_array_length(properties); j++)
        {
            struct ZenitPropertyNode *prop = properties[j];

            // Visit the property's value
            visit_node(ctx, prop->value);

            // Add a temporal symbol for the property with type == NONE (infer pass will update it)
            zenit_utils_new_tmp_symbol(ctx->program, (struct ZenitNode*) prop, zenit_type_pool_register(&ctx->program->type_pool, zenit_type_none_new()));
        }
        fl_array_free(properties);
    }
    fl_array_free(names);
}

/*
 * Function: visit_variable_node
 *  In this function we need to insert a new <struct ZenitSymbol> object in the symbol table. In case the
 *  symbol already exists, we need to add an error to the <struct ZenitContext> object
 *
 * Parameters:
 *  <struct ZenitContext> *ctx - Context object
 *  <struct ZenitVariableNode> *variable_node - Visited node
 *
 * Returns:
 *  struct ZenitSymbol* - The new symbol introduced by the variable declaration or NULL on error
 *
 */
static struct ZenitSymbol* visit_variable_node(struct ZenitContext *ctx, struct ZenitVariableNode *variable_node)
{
    // We don't care about the returned symbol -if any- at this pass
    struct ZenitSymbol *rhs_symbol = visit_node(ctx, variable_node->rvalue);

    // If the symbol already exists add an error
    if (zenit_program_has_symbol(ctx->program, variable_node->name))
    {
        zenit_context_error(ctx, variable_node->base.location, ZENIT_ERROR_DUPLICATED_SYMBOL, "Cannot redefine symbol '%s'", variable_node->name);
        return NULL;
    }    

    // Visit the attributes and its properties
    visit_attribute_node_map(ctx, &variable_node->attributes);

    // If the variable has a type hint, we should honor it and let the inference and type check passes to take care of it.
    struct ZenitTypeInfo *typeinfo = NULL;
    
    if (variable_node->type_decl)
        typeinfo = build_type_info_from_declaration(ctx->program, variable_node->type_decl);
    else if (rhs_symbol)
        typeinfo = rhs_symbol->typeinfo;
    else
        typeinfo = zenit_type_pool_register(&ctx->program->type_pool, (struct ZenitTypeInfo*) zenit_type_none_new());

    // Create and insert the symbol in the table
    return zenit_program_add_symbol(ctx->program, zenit_symbol_new(variable_node->name, typeinfo));
}

/*
 * Function: visit_node
 *  This function selects the visitor function based on the node's type
 *  and calls the function.
 *
 * Parameters:
 *  ctx - Context object
 *  <struct ZenitNode> *node - Node to visit
 *
 * Returns:
 *  struct ZenitSymbol* - A valid symbol if the visited node (or nodes) defines one or NULL
 *
 */
static struct ZenitSymbol* visit_node(struct ZenitContext *ctx, struct ZenitNode *node)
{
    return symbol_resolvers[node->type](ctx, node);
}


/*
 * Function: zenit_resolve_symbols
 *  We just iterate over the declarations visiting each node
 */
bool zenit_resolve_symbols(struct ZenitContext *ctx)
{
    if (!ctx || !ctx->ast || !ctx->ast->decls)
        return false;

    size_t errors = zenit_context_error_count(ctx);

    for (size_t i=0; i < fl_array_length(ctx->ast->decls); i++)
        visit_node(ctx, ctx->ast->decls[i]);

    return errors == zenit_context_error_count(ctx);
}
