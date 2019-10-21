#include "resolve.h"
#include "symbol.h"

/*
 * Type: CenitSymbolResolver
 *  A resolver function takes the <CenitSymbolTable> and a <CenitNode> and
 *  populates the table (if needed) with symbol information.
 *  The function returns a <CenitSymbol> if the node updates the table or NULL
 *  if there is nothing to update
 */
typedef CenitSymbol*(*CenitSymbolResolver)(CenitContext *ctx, CenitNode *node);

// Visitor functions
static CenitSymbol* visit_node(CenitContext *ctx, CenitNode *node);
static CenitSymbol* visit_nothing(CenitContext *ctx, CenitNode *node);
static CenitSymbol* visit_variable(CenitContext *ctx, CenitNode *node);

/*
 * Variable: resolvers
 *  An array indexed with a <CenitNodeType> to get a <CenitSymbolResolver> function
 */
static const CenitSymbolResolver resolvers[] = {
    // These nodes introduce new symbols into
    // the symbol table
    [CENIT_NODE_VARIABLE]   = &visit_variable,

    // These nodes do not play a role in this pass
    [CENIT_NODE_LITERAL]    = &visit_nothing,
    [CENIT_NODE_ARRAY_INIT] = &visit_nothing,
    
};

/*
 * Function: visit_nothing
 *  Some leafs in the <CenitAst> do not perform symbol-related actions so this
 *  function simply returns NULL
 *
 * Parameters:
 *  ctx - <CenitContext> object
 *  node - Node object
 *
 * Returns:
 *  CenitSymbol* - This function always returns NULL
 *
 */
static CenitSymbol* visit_nothing(CenitContext *ctx, CenitNode *node)
{
    return NULL;
}

/*
 * Function: visit_variable
 *  In this function we need to insert a new <CenitSymbol> object in the symbol table. In case the
 *  symbol already exists, we need to add an error to the <CenitContext> object
 *
 * Parameters:
 *  ctx - <CenitContext> object
 *  node - Expected pointer to a <CenitVariableNode> object
 *
 * Returns:
 *  CenitSymbol* - The new symbol introduced by the variable declaration or NULL on error
 *
 */
static CenitSymbol* visit_variable(CenitContext *ctx, CenitNode *node)
{
    CenitVariableNode *var_decl = (CenitVariableNode*)node;

    // We don't care about the returned symbol -if any- at this pass
    visit_node(ctx, var_decl->value);

    // If the symbol already exists add an error
    if (cenit_symtable_has(&ctx->symtable, var_decl->name))
    {
        cenit_context_error(ctx, var_decl->base.location, CENIT_ERROR_DUPLICATED_SYMBOL, "Cannot redefine symbol '%s'", var_decl->name);
        return NULL;
    }

    // Create and insert the symbol in the table
    return cenit_symtable_add(&ctx->symtable, cenit_symbol_new(var_decl->name, &var_decl->typeinfo));
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
 *  CenitSymbol* - A valid symbol if the visited node (or nodes) defines one
 *                  or NULL
 *
 */
static CenitSymbol* visit_node(CenitContext *ctx, CenitNode *node)
{
    return resolvers[node->type](ctx, node);
}


/*
 * Function: cenit_resolve_symbols
 *  We just iterate over the declarations visiting each node
 */
bool cenit_resolve_symbols(CenitContext *ctx)
{
    if (!ctx || !ctx->ast || !ctx->ast->decls)
        return false;

    size_t errors = cenit_context_error_count(ctx);

    for (size_t i=0; i < fl_array_length(ctx->ast->decls); i++)
        visit_node(ctx, ctx->ast->decls[i]);

    return errors == cenit_context_error_count(ctx);
}
