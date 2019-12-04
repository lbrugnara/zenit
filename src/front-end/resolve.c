#include "program.h"
#include "resolve.h"
#include "symbol.h"

/*
 * Type: ZenitSymbolResolver
 *  A resolver function takes the <struct ZenitSymbolTable> and a <struct ZenitNode> and
 *  populates the table (if needed) with symbol information.
 *  The function returns a <struct ZenitSymbol> if the node updates the table or NULL
 *  if there is nothing to update
 */
typedef struct ZenitSymbol*(*ZenitSymbolResolver)(struct ZenitContext *ctx, struct ZenitNode *node);

// Visitor functions
static struct ZenitSymbol* visit_node(struct ZenitContext *ctx, struct ZenitNode *node);
static struct ZenitSymbol* visit_nothing(struct ZenitContext *ctx, struct ZenitNode *node);
static struct ZenitSymbol* visit_variable(struct ZenitContext *ctx, struct ZenitNode *node);
static struct ZenitSymbol* visit_array_initializer(struct ZenitContext *ctx, struct ZenitNode *node);
static struct ZenitSymbol* visit_identifier(struct ZenitContext *ctx, struct ZenitNode *node);
static struct ZenitSymbol* visit_unary_ref(struct ZenitContext *ctx, struct ZenitNode *node);
static struct ZenitSymbol* visit_cast(struct ZenitContext *ctx, struct ZenitNode *node);

/*
 * Variable: resolvers
 *  An array indexed with a <enum ZenitNodeType> to get a <ZenitSymbolResolver> function
 */
static const ZenitSymbolResolver resolvers[] = {
    // These nodes introduce new symbols into the symbol table or retrieve symbols from it
    [ZENIT_NODE_VARIABLE]   = &visit_variable,
    [ZENIT_NODE_IDENTIFIER] = &visit_identifier,
    [ZENIT_NODE_ARRAY_INIT] = &visit_array_initializer,
    [ZENIT_NODE_UNARY_REF]  = &visit_unary_ref,
    [ZENIT_NODE_CAST]       = &visit_cast,
    [ZENIT_NODE_LITERAL]    = &visit_nothing,
    
};

/*
 * Function: visit_nothing
 *  Some leafs in the <struct ZenitAst> do not perform symbol-related actions so this
 *  function simply returns NULL
 *
 * Parameters:
 *  ctx - <struct ZenitContext> object
 *  node - Node object
 *
 * Returns:
 *  struct ZenitSymbol* - This function always returns NULL
 *
 */
static struct ZenitSymbol* visit_nothing(struct ZenitContext *ctx, struct ZenitNode *node)
{
    return NULL;
}

static struct ZenitSymbol* visit_cast(struct ZenitContext *ctx, struct ZenitNode *node)
{
    struct ZenitCastNode *cast_node = (struct ZenitCastNode*)node;

    visit_node(ctx, cast_node->expression);

    return NULL;
}

static struct ZenitSymbol* visit_identifier(struct ZenitContext *ctx, struct ZenitNode *node)
{
    struct ZenitIdentifierNode *id_node = (struct ZenitIdentifierNode*)node;

    // If the symbol exists we return it to the caller
    if (zenit_program_has_symbol(ctx->program, id_node->name))
        return zenit_program_get_symbol(ctx->program, id_node->name);

    // By now, if the identifier does not exist, we add an error (we are not declaring variables or types at the moment, so a missing identifier
    // is a not defined variable)
    zenit_context_error(ctx, id_node->base.location, ZENIT_ERROR_MISSING_SYMBOL, "Identifier %s is not defined.", id_node->name);

    return NULL;
}

static struct ZenitSymbol* visit_unary_ref(struct ZenitContext *ctx, struct ZenitNode *node)
{
    struct ZenitUnaryRefNode *ref_node = (struct ZenitUnaryRefNode*)node;
    visit_node(ctx, ref_node->expression);
    return NULL;
}

static struct ZenitSymbol* visit_array_initializer(struct ZenitContext *ctx, struct ZenitNode *node)
{
    struct ZenitArrayNode *array = (struct ZenitArrayNode*)node;

    for (size_t i=0; i < fl_array_length(array->elements); i++)
        visit_node(ctx, array->elements[i]);

    return NULL;
}

/*
 * Function: visit_variable
 *  In this function we need to insert a new <struct ZenitSymbol> object in the symbol table. In case the
 *  symbol already exists, we need to add an error to the <struct ZenitContext> object
 *
 * Parameters:
 *  ctx - <struct ZenitContext> object
 *  node - Expected pointer to a <struct ZenitVariableNode> object
 *
 * Returns:
 *  struct ZenitSymbol* - The new symbol introduced by the variable declaration or NULL on error
 *
 */
static struct ZenitSymbol* visit_variable(struct ZenitContext *ctx, struct ZenitNode *node)
{
    struct ZenitVariableNode *var_decl = (struct ZenitVariableNode*)node;

    // We don't care about the returned symbol -if any- at this pass
    visit_node(ctx, var_decl->value);

    // If the symbol already exists add an error
    if (zenit_program_has_symbol(ctx->program, var_decl->name))
    {
        zenit_context_error(ctx, var_decl->base.location, ZENIT_ERROR_DUPLICATED_SYMBOL, "Cannot redefine symbol '%s'", var_decl->name);
        return NULL;
    }

    const char **names = zenit_attribute_node_map_keys(&var_decl->attributes);
    for (size_t i=0; i < fl_array_length(names); i++)
    {
        struct ZenitAttributeNode *attr = zenit_attribute_node_map_get(&var_decl->attributes, names[i]);

        struct ZenitPropertyNode **properties = zenit_property_node_map_values(&attr->properties);
        for (size_t j=0; j < fl_array_length(properties); j++)
        {
            struct ZenitPropertyNode *prop = properties[i];
            visit_node(ctx, prop->value);
        }
        fl_array_free(properties);
    }
    fl_array_free(names);

    // Create and insert the symbol in the table
    return zenit_program_add_symbol(ctx->program, zenit_symbol_new(var_decl->name, &var_decl->base.typeinfo));
}

/*
 * Function: visit_node
 *  This function selects the visitor function based on the node's type
 *  and calls the function.
 *
 * Parameters:
 *  ctx - Context object
 *  node - Node to visit
 *
 * Returns:
 *  struct ZenitSymbol* - A valid symbol if the visited node (or nodes) defines one or NULL
 *
 */
static struct ZenitSymbol* visit_node(struct ZenitContext *ctx, struct ZenitNode *node)
{
    return resolvers[node->type](ctx, node);
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
