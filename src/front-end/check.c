#include "infer.h"
#include "symbol.h"

/*
 * Function: is_type_defined
 *  If the type is a native type, we are sure it is a defined type, but if the
 *  type is a CUSTOM one, we need to check if the type is registered in the symbol
 *  table
 *
 * Parameters:
 *  symtable - Symbol table
 *  typeinfo - Type information
 *
 * Returns:
 *  bool - *true* if the type is a native type or a custom type defined in the symbol
 *          table, otherwise this function returns *false*.
 */
static bool is_type_defined(CenitSymbolTable *symtable, CenitTypeInfo *typeinfo)
{
    if (typeinfo->type != CENIT_TYPE_CUSTOM)
        return true;

    return cenit_symtable_has(symtable, typeinfo->name);
}

/*
 * Type: CenitTypeChecker
 *  A checker function takes a symbol from the <CenitSymbolTable> and 
 *  and makes sure its type is valid in the context that is determined
 *  by the <CenitNode>.
 *  The function returns a <CenitTypeInfo> pointer with the checked type information
 *  or NULL on error where the caller should know how to handle that and should keep
 *  propagating the error.
 */
typedef CenitTypeInfo*(*CenitTypeChecker)(CenitContext *ctx, CenitNode *node);

// Visitor functions
static CenitTypeInfo* visit_node(CenitContext *ctx, CenitNode *node);
static CenitTypeInfo* visit_literal(CenitContext *ctx, CenitNode *node);
static CenitTypeInfo* visit_variable(CenitContext *ctx, CenitNode *node);
static CenitTypeInfo* visit_array_initializer(CenitContext *ctx, CenitNode *node);

/*
 * Variable: checkers
 *  An array indexed with a <CenitNodeType> to get a <CenitTypeChecker> function
 */
static const CenitTypeChecker checkers[] = {
    [CENIT_NODE_LITERAL]    = &visit_literal,
    [CENIT_NODE_VARIABLE]   = &visit_variable,
    [CENIT_NODE_ARRAY_INIT] = &visit_array_initializer,
};

/*
 * Function: visit_literal
 *  The literal node doesn't need to check anything, it just
 *  returns its type information
 *
 * Parameters:
 *  ctx - Context object
 *  node - Node to visit
 *
 * Returns:
 *  CenitTypeInfo* - The literal's type information
 */
static CenitTypeInfo* visit_literal(CenitContext *ctx, CenitNode *node)
{
    CenitLiteralNode *literal = (CenitLiteralNode*)node;
    return &literal->typeinfo;
}

/*
 * Function: visit_array_initializer
 *  To make sure the array initialization is valid, we need to visit every
 *  element to check it against the array's type.
 *
 * Parameters:
 *  ctx - Context object
 *  node - Node to visit
 *
 * Returns:
 *  CenitTypeInfo* - The arrays's type information
 */
static CenitTypeInfo* visit_array_initializer(CenitContext *ctx, CenitNode *node)
{
    CenitArrayInitNode *array = (CenitArrayInitNode*)node;

    // The array type is inferred in the inference pass, so we have information
    // about it, but it can be a custom type that doesn't exist in the symbol
    // table
    bool array_type_defined = is_type_defined(&ctx->symtable, &array->typeinfo);

    for (size_t i=0; i < fl_array_length(array->values); i++)
    {
        CenitTypeInfo* elem_typeinfo = visit_node(ctx, array->values[i]);

        // If the type check fails, we add an error only if the array type is defined
        // because of not we might be targeting a false positive error
        if (array_type_defined && elem_typeinfo->type != array->typeinfo.type)
        {
            cenit_context_error(ctx, array->values[i]->location, CENIT_ERROR_TYPE_MISSMATCH, 
                "Cannot convert from type '%s' to '%s'", cenit_type_to_string(elem_typeinfo), cenit_type_to_string(&array->typeinfo));
        }
    }

    // We return the type of the array initializer
    return &array->typeinfo;
}

/*
 * Function: visit_variable
 *  The variable visitor makes sure the variable type exists and also makes
 *  sure the right-hand side expression is compatible with it
 *
 * Parameters:
 *  ctx - Context object
 *  node - Node to visit
 *
 * Returns:
 *  CenitTypeInfo* - The variable's type information
 */
static CenitTypeInfo* visit_variable(CenitContext *ctx, CenitNode *node)
{
    CenitVariableNode *var_decl = (CenitVariableNode*)node;

    // We get the variable's symbol
    CenitSymbol *symbol = cenit_symtable_get(&ctx->symtable, var_decl->name);

    // Check if the variable's type is native or it is registered in the symbol table
    bool var_type_defined = is_type_defined(&ctx->symtable, &symbol->typeinfo);

    // If the variable type is missing, we add an error
    if (!var_type_defined)
    {
        cenit_context_error(ctx, var_decl->base.location, CENIT_ERROR_MISSING_SYMBOL, 
            "Type '%s' is not defined", cenit_type_to_base_string(&var_decl->typeinfo));
    }

    // We visit the right-hand side expression to do type checking in it
    CenitTypeInfo* rhs_type = visit_node(ctx, var_decl->value);
    
    // We check types to make sure the assignment is valid, but we do it only if
    // the variable type is valid, because if not, we might be targeting a false-positive
    // error
    if (var_type_defined && !cenit_type_equals(&symbol->typeinfo, rhs_type))
    {
        cenit_context_error(ctx, var_decl->base.location, CENIT_ERROR_TYPE_MISSMATCH, 
                "Cannot convert from type '%s' to '%s'", cenit_type_to_string(rhs_type), cenit_type_to_string(&symbol->typeinfo));
    }

    // The type information returned is always the one from the variable's symbol
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
 *  CenitTypeInfo* - A valid type information object or NULL
 */
static CenitTypeInfo* visit_node(CenitContext *ctx, CenitNode *node)
{
    return checkers[node->type](ctx, node);
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
