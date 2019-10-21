#include "infer.h"
#include "symbol.h"

/*
 * Type: CenitTypeInferrer
 *  An inferrer function takes a symbol from the <CenitSymbolTable> and updates
 *  it -if needed- with type information the symbol could be missing.
 *  The function returns a <CenitTypeInfo> with the type retrieved from the symbol
 *  or inferred from the context.
 */
typedef CenitTypeInfo*(*CenitTypeInferrer)(CenitContext *ctx, CenitNode *node);

// Visitor functions
static CenitTypeInfo* visit_node(CenitContext *ctx, CenitNode *node);
static CenitTypeInfo* visit_literal(CenitContext *ctx, CenitNode *node);
static CenitTypeInfo* visit_variable(CenitContext *ctx, CenitNode *node);
static CenitTypeInfo* visit_array_initializer(CenitContext *ctx, CenitNode *node);

/*
 * Variable: inferrers
 *  An array indexed with a <CenitNodeType> to get a <CenitTypeInferrer> function
 */
static const CenitTypeInferrer inferrers[] = {
    [CENIT_NODE_LITERAL]    = &visit_literal,
    [CENIT_NODE_VARIABLE]   = &visit_variable,
    [CENIT_NODE_ARRAY_INIT] = &visit_array_initializer,
};

/*
 * Function: visit_literal
 *  A literal node always has a type therefore we just return it
 *
 * Parameters:
 *  ctx - Context object
 *  node - Literal node
 *
 * Returns:
 *  CenitTypeInfo* - The intrinsic type of the literal node
 *
 */
static CenitTypeInfo* visit_literal(CenitContext *ctx, CenitNode *node)
{
    return &((CenitLiteralNode*)node)->typeinfo;
}

/*
 * Function: visit_array_initializer
 *  The array initializer always has type NONE before this pass, so we here update it
 *  to be the type of the first element within the array that has a type different from
 *  NONE.
 *
 * Parameters:
 *  ctx - Context object
 *  node - Array initializer node
 *
 * Returns:
 *  CenitTypeInfo* - The inferred type of the array
 *
 */
static CenitTypeInfo* visit_array_initializer(CenitContext *ctx, CenitNode *node)
{
    CenitArrayInitNode *array = (CenitArrayInitNode*)node;

    for (size_t i=0; i < fl_array_length(array->values); i++)
    {
        CenitTypeInfo *elem_type = visit_node(ctx, array->values[i]);

        if (array->typeinfo.type == CENIT_TYPE_NONE)
            array->typeinfo.type = elem_type->type;
    }

    return &array->typeinfo;
}

/*
 * Function: visit_variable
 *  This function infers the variable type from the right-hand side expression
 *  or the other way around. If the type of both elements are <CENIT_TYPE_NON> 
 *  this function adds a new error because that means we can't infer their types.
 *  If both types are defined bu are not equals, we don't need to do anything here,
 *  the type checking pass will take care of it.
 *
 * Parameters:
 *  ctx - Context object
 *  node - Variable declaration node
 *
 * Returns:
 *  CenitTypeInfo* - The declared or inferred type of the variable
 *
 */
static CenitTypeInfo* visit_variable(CenitContext *ctx, CenitNode *node)
{
    CenitVariableNode *var_decl = (CenitVariableNode*)node;
    
    // We need the symbol we introduced in the <cenit_resolve_symbols> pass
    CenitSymbol *symbol = cenit_symtable_get(&ctx->symtable, var_decl->name);

    // We need to get the type of the right-hand side expression
    CenitTypeInfo *rhs_type = visit_node(ctx, var_decl->value);

    if (symbol->typeinfo.type == CENIT_TYPE_NONE && rhs_type->type != CENIT_TYPE_NONE)
    {
        // If symbol's type is none, we use the rhs type
        cenit_type_copy(&symbol->typeinfo, rhs_type);
    }
    else if (symbol->typeinfo.type != CENIT_TYPE_NONE && rhs_type->type == CENIT_TYPE_NONE)
    {
        // If the rhs type is none, we update it using the variable's type
        cenit_type_copy(rhs_type, &symbol->typeinfo);
    }
    else if (symbol->typeinfo.type == CENIT_TYPE_NONE && rhs_type->type == CENIT_TYPE_NONE)
    {
        // If both are none, it is an error for us
        cenit_context_error(ctx, var_decl->base.location, CENIT_ERROR_INFERENCE, 
                "Cannot infer type of variable '%s' from the right-hand expression. Try making it explicit in the variable.", var_decl->name);
    }

    // Always the declared or inferred type. In case of error, we honor the variable type
    return &symbol->typeinfo;
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
 *  CenitTypeInfo* - A pointer to an object containing type information
 *                   or NULL
 *
 */
static CenitTypeInfo* visit_node(CenitContext *ctx, CenitNode *node)
{
    return inferrers[node->type](ctx, node);
}

/*
 * Function: cenit_infer_symbols
 *  We just iterate over the declarations visiting each node
 */
bool cenit_infer_types(CenitContext *ctx)
{
    if (!ctx || !ctx->ast || !ctx->ast->decls)
        return false;

    size_t errors = cenit_context_error_count(ctx);

    for (size_t i=0; i < fl_array_length(ctx->ast->decls); i++)
        visit_node(ctx, ctx->ast->decls[i]);

    return errors == cenit_context_error_count(ctx);
}
