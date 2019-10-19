#include "infer.h"
#include "symbol.h"

/*
 * Type: CenitChecker
 *  A checker function takes a symbol from the <CenitSymbolTable> and 
 *  and makes sure its type is valid in the context that is determined
 *  by the <CenitNode>.
 *  The function returns a <CenitTypeInfo> pointer with the checked type information
 *  or NULL on error where the caller should know how to handle that and should keep
 *  propagating the error.
 */
typedef CenitTypeInfo*(*CenitChecker)(CenitContext *ctx, CenitNode *node);

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
 *  CenitTypeInfo* - A valid symbol if the visited node (or nodes) defines one
 *                  or NULL
 *
 */
static CenitTypeInfo* visit_node(CenitContext *ctx, CenitNode *node);
static CenitTypeInfo* visit_literal(CenitContext *ctx, CenitNode *node);
static CenitTypeInfo* visit_variable(CenitContext *ctx, CenitNode *node);
static CenitTypeInfo* visit_array_initializer(CenitContext *ctx, CenitNode *node);

static const CenitChecker inferrers[] = {
    [CENIT_NODE_LITERAL] = &visit_literal,
    [CENIT_NODE_VARIABLE] = &visit_variable,
    [CENIT_NODE_ARRAY_INIT] = &visit_array_initializer,
};

static CenitTypeInfo* visit_literal(CenitContext *ctx, CenitNode *node)
{
    CenitLiteralNode *literal = (CenitLiteralNode*)node;
    return &literal->typeinfo;
}

static CenitTypeInfo* visit_array_initializer(CenitContext *ctx, CenitNode *node)
{
    CenitArrayInitNode *array = (CenitArrayInitNode*)node;

    for (size_t i=0; i < fl_array_length(array->values); i++)
    {
        CenitTypeInfo* elem_typeinfo = visit_node(ctx, array->values[i]);

        if (!cenit_type_equals(elem_typeinfo, &array->typeinfo))
        {
            // TODO: Handle error
        }
    }

    return &array->typeinfo;
}

static CenitTypeInfo* visit_variable(CenitContext *ctx, CenitNode *node)
{
    CenitVariableNode *var_decl = (CenitVariableNode*)node;

    CenitTypeInfo* rhs_type = visit_node(ctx, var_decl->value);

    CenitSymbol *symbol = cenit_symtable_get(&ctx->symtable, var_decl->name);

    if (!cenit_type_equals(&symbol->typeinfo, rhs_type))
    {
        // TODO: Handle error
    }

    return &symbol->typeinfo;
}

static CenitTypeInfo* visit_node(CenitContext *ctx, CenitNode *node)
{
    return inferrers[node->type](ctx, node);
}

/*
 * Function: cenit_infer_symbols
 *  We just iterate over the declarations visiting each node
 */
bool cenit_check_types(CenitContext *ctx)
{
    if (!ctx || !ctx->ast || !ctx->ast->decls)
        return false;

    size_t errors = cenit_context_error_count(ctx);

    for (size_t i=0; i < fl_array_length(ctx->ast->decls); i++)
        visit_node(ctx, ctx->ast->decls[i]);

    return errors == cenit_context_error_count(ctx);
}
