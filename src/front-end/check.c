#include "check.h"
#include "program.h"
#include "symbol.h"

/*
 * Function: is_type_defined
 *  If the type is a native type, we are sure it is a defined type, but if the
 *  type is a CUSTOM one, we need to check if the type is registered in the symbol
 *  table
 *
 * Parameters:
 *  program - Program object
 *  typeinfo - Type information
 *
 * Returns:
 *  bool - *true* if the type is a native type or a custom type defined in the symbol
 *          table, otherwise this function returns *false*.
 */
static bool is_type_defined(struct ZenitProgram *program, struct ZenitTypeInfo *typeinfo)
{
    if (typeinfo->type != ZENIT_TYPE_CUSTOM)
        return true;

    return zenit_program_has_symbol(program, typeinfo->name)
            || zenit_program_has_global_symbol(program, typeinfo->name);
}

/*
 * Type: ZenitTypeChecker
 *  A checker function takes a symbol from the <struct ZenitSymbolTable> and 
 *  and makes sure its type is valid in the context that is determined
 *  by the <struct ZenitNode>.
 *  The function returns a <struct ZenitTypeInfo> pointer with the checked type information
 *  or NULL on error where the caller should know how to handle that and should keep
 *  propagating the error.
 */
typedef struct ZenitTypeInfo*(*ZenitTypeChecker)(struct ZenitContext *ctx, struct ZenitNode *node);

// Visitor functions
static struct ZenitTypeInfo* visit_node(struct ZenitContext *ctx, struct ZenitNode *node);
static struct ZenitTypeInfo* visit_literal(struct ZenitContext *ctx, struct ZenitNode *node);
static struct ZenitTypeInfo* visit_variable(struct ZenitContext *ctx, struct ZenitNode *node);
static struct ZenitTypeInfo* visit_array_initializer(struct ZenitContext *ctx, struct ZenitNode *node);
static struct ZenitTypeInfo* visit_identifier(struct ZenitContext *ctx, struct ZenitNode *node);
static struct ZenitTypeInfo* visit_unary_ref(struct ZenitContext *ctx, struct ZenitNode *node);
static struct ZenitTypeInfo* visit_cast(struct ZenitContext *ctx, struct ZenitNode *node);

/*
 * Variable: checkers
 *  An array indexed with a <enum ZenitNodeType> to get a <ZenitTypeChecker> function
 */
static const ZenitTypeChecker checkers[] = {
    [ZENIT_NODE_LITERAL]    = &visit_literal,
    [ZENIT_NODE_VARIABLE]   = &visit_variable,
    [ZENIT_NODE_ARRAY_INIT] = &visit_array_initializer,
    [ZENIT_NODE_IDENTIFIER] = &visit_identifier,
    [ZENIT_NODE_UNARY_REF]  = &visit_unary_ref,
    [ZENIT_NODE_CAST]       = &visit_cast,
};

static struct ZenitTypeInfo* visit_cast(struct ZenitContext *ctx, struct ZenitNode *node)
{
    struct ZenitCastNode *cast = (struct ZenitCastNode*)node;

    // If the cast is implicit, we need to make sure we can up-cast the expression to the requested type
    if ((cast->implicit && !zenit_type_can_assign(&cast->base.typeinfo, &cast->expression->typeinfo))
        // HACK: If the cast is explicit, we can check if the other way around works for these types, in that
        // case we know it is safe to "truncate" the type :grimming:
        || (!cast->implicit && !zenit_type_can_cast(&cast->base.typeinfo, &cast->expression->typeinfo))
    )
    {
        zenit_context_error(ctx, cast->base.location, ZENIT_ERROR_TYPE_MISSMATCH, "Cannot % from type '%s' to '%s'", 
                cast->implicit ? "implicitly cast" : "cast", 
                zenit_type_to_string(&cast->expression->typeinfo),
                zenit_type_to_string(&cast->base.typeinfo)
        );
    }

    return &cast->base.typeinfo;
}

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
 *  struct ZenitTypeInfo* - The literal's type information
 */
static struct ZenitTypeInfo* visit_literal(struct ZenitContext *ctx, struct ZenitNode *node)
{
    struct ZenitLiteralNode *literal = (struct ZenitLiteralNode*)node;
    return &literal->base.typeinfo;
}

/*
 * Function: visit_identifier
 *  It just returns the type information of the identifier
 *
 * Parameters:
 *  ctx - Context object
 *  node - Node to visit
 *
 * Returns:
 *  struct ZenitTypeInfo* - The identifier type information
 */
static struct ZenitTypeInfo* visit_identifier(struct ZenitContext *ctx, struct ZenitNode *node)
{
    struct ZenitIdentifierNode *id_node = (struct ZenitIdentifierNode*)node;
    return &zenit_program_get_symbol(ctx->program, id_node->name)->typeinfo;
}

/*
 * Function: visit_unary_ref
 *  Check the referenced expression is a valid expression to take its address from
 *
 * Parameters:
 *  ctx - Context object
 *  node - Node to visit
 *
 * Returns:
 *  struct ZenitTypeInfo* - The reference type information
 */
static struct ZenitTypeInfo* visit_unary_ref(struct ZenitContext *ctx, struct ZenitNode *node)
{
    struct ZenitUnaryRefNode *ref_node = (struct ZenitUnaryRefNode*)node;
    struct ZenitTypeInfo *expr_info = visit_node(ctx, ref_node->expression);

    if (expr_info->is_ref)
    {
        zenit_context_error(ctx, ref_node->base.location, ZENIT_ERROR_INVALID_REF, 
                "Cannot take a reference to another reference.");
    }

    return &ref_node->base.typeinfo;
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
 *  struct ZenitTypeInfo* - The arrays's type information
 */
static struct ZenitTypeInfo* visit_array_initializer(struct ZenitContext *ctx, struct ZenitNode *node)
{
    struct ZenitArrayNode *array = (struct ZenitArrayNode*)node;

    // The array type is inferred in the inference pass, so we have information
    // about it, but it can be a custom type that doesn't exist in the symbol
    // table
    bool array_type_defined = is_type_defined(ctx->program, &array->base.typeinfo);

    for (size_t i=0; i < fl_array_length(array->elements); i++)
    {
        struct ZenitTypeInfo* elem_typeinfo = visit_node(ctx, array->elements[i]);

        // If the type check fails, we add an error only if the array type is defined
        // because of not we might be targeting a false positive error
        if (array_type_defined && elem_typeinfo->type != array->base.typeinfo.type)
        {
            zenit_context_error(ctx, array->elements[i]->location, ZENIT_ERROR_TYPE_MISSMATCH, 
                "Cannot convert from type '%s' to '%s'", zenit_type_to_string(elem_typeinfo), zenit_type_to_string(&array->base.typeinfo));
        }
    }

    // We return the type of the array initializer
    return &array->base.typeinfo;
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
 *  struct ZenitTypeInfo* - The variable's type information
 */
static struct ZenitTypeInfo* visit_variable(struct ZenitContext *ctx, struct ZenitNode *node)
{
    struct ZenitVariableNode *var_decl = (struct ZenitVariableNode*)node;

    // We get the variable's symbol
    struct ZenitSymbol *symbol = zenit_program_get_symbol(ctx->program, var_decl->name);

    // Check if the variable's type is native or it is registered in the symbol table
    bool var_type_defined = is_type_defined(ctx->program, &symbol->typeinfo);

    // If the variable type is missing, we add an error
    if (!var_type_defined)
    {
        zenit_context_error(ctx, var_decl->base.location, ZENIT_ERROR_MISSING_SYMBOL, 
            "Type '%s' is not defined", zenit_type_to_base_string(&var_decl->base.typeinfo));
    }

    // We visit the right-hand side expression to do type checking in it
    struct ZenitTypeInfo* rhs_type = visit_node(ctx, var_decl->value);
    
    // We check types to make sure the assignment is valid, but we do it only if
    // the variable type is valid, because if not, we might be targeting a false-positive
    // error
    if (var_type_defined && !zenit_type_can_assign(&symbol->typeinfo, rhs_type))
    {
        zenit_context_error(ctx, var_decl->base.location, ZENIT_ERROR_TYPE_MISSMATCH, 
                "Cannot convert from type '%s' to '%s'", zenit_type_to_string(rhs_type), zenit_type_to_string(&symbol->typeinfo));
    }

    // FIXME: Visit the attributes

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
 *  struct ZenitTypeInfo* - A valid type information object or NULL
 */
static struct ZenitTypeInfo* visit_node(struct ZenitContext *ctx, struct ZenitNode *node)
{
    return checkers[node->type](ctx, node);
}

/*
 * Function: zenit_infer_symbols
 *  We just iterate over the declarations visiting each node
 */
bool zenit_check_types(struct ZenitContext *ctx)
{
    if (!ctx || !ctx->ast || !ctx->ast->decls)
        return false;

    size_t errors = zenit_context_error_count(ctx);

    for (size_t i=0; i < fl_array_length(ctx->ast->decls); i++)
        visit_node(ctx, ctx->ast->decls[i]);

    return errors == zenit_context_error_count(ctx);
}
