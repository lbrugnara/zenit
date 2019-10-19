#include "infer.h"
#include "symbol.h"

/*
 * Type: CenitInferrer
 *  An inferrer function takes a symbol from the <CenitSymbolTable> and updates
 *  it -if needed- with type information the symbol could be missing.
 *  The function returns a <CenitTypeInfo> with the type retrieved from the symbol
 *  or inferred from the context.
 */
typedef CenitTypeInfo(*CenitInferrer)(CenitContext *ctx, CenitNode *node);

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
 *  CenitTypeInfo - The type information retrieved in the context of the
 *                  visited node
 */
static CenitTypeInfo visit_node(CenitContext *ctx, CenitNode *node);


/*
 * Function: visit_literal
 *  This function returns the from the literal value so it doesn't actually infer
 *  the type, but being literals are a leaf in the AST, we "propagate" the type
 *  up
 *
 * Parameters:
 *  ctx - Context object
 *  node - Node to visit
 *
 * Returns:
 *  CenitTypeInfo - The type information retrieved in the context of the
 *                  visited node
 */
static CenitTypeInfo visit_literal(CenitContext *ctx, CenitNode *node);


/*
 * Function: visit_variable
 *  If the variable does not have its type information, we retrieve it from the 
 *  right-hand side expression
 *
 * Parameters:
 *  ctx - Context object
 *  node - Node to visit
 *
 * Returns:
 *  CenitTypeInfo - The type information retrieved in the context of the
 *                  visited node
 */
static CenitTypeInfo visit_variable(CenitContext *ctx, CenitNode *node);


/*
 * Function: visit_array_initializer
 *  The function iterates over the array to recursively infer types of
 *  its elements.
 *  For simplicity, we assume the type of the array is equals to the
 *  type of the first element with a type different from <CENIT_TYPE_NONE>
 *
 * Parameters:
 *  ctx - Context object
 *  node - Node to visit
 *
 * Returns:
 *  CenitTypeInfo - The type information retrieved in the context of the
 *                  visited node
 */
static CenitTypeInfo visit_array_initializer(CenitContext *ctx, CenitNode *node);

static const CenitInferrer inferrers[] = {
    [CENIT_NODE_LITERAL] = &visit_literal,
    [CENIT_NODE_VARIABLE] = &visit_variable,
    [CENIT_NODE_ARRAY_INIT] = &visit_array_initializer,
};

static CenitTypeInfo visit_literal(CenitContext *ctx, CenitNode *node)
{
    CenitLiteralNode *literal = (CenitLiteralNode*)node;
    return literal->typeinfo;
}

static CenitTypeInfo visit_array_initializer(CenitContext *ctx, CenitNode *node)
{
    CenitArrayInitNode *array = (CenitArrayInitNode*)node;

    for (size_t i=0; i < fl_array_length(array->values); i++)
    {
        CenitTypeInfo elem_type = visit_node(ctx, array->values[i]);

        if (array->typeinfo.type == CENIT_TYPE_NONE)
            array->typeinfo.type = elem_type.type;
    }

    return array->typeinfo;
}

static CenitTypeInfo visit_variable(CenitContext *ctx, CenitNode *node)
{
    CenitVariableNode *var_decl = (CenitVariableNode*)node;

    CenitTypeInfo rhs_type = visit_node(ctx, var_decl->value);

    CenitSymbol *symbol = cenit_symtable_get(&ctx->symtable, var_decl->name);

    if (symbol->typeinfo.type == CENIT_TYPE_NONE)
        cenit_type_copy(&symbol->typeinfo, &rhs_type);

    return symbol->typeinfo;
}

static CenitTypeInfo visit_node(CenitContext *ctx, CenitNode *node)
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
