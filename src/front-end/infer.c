#include "infer.h"
#include "program.h"
#include "symbol.h"

/*
 * Type: ZenitTypeInferrer
 *  An inferrer function takes a symbol from the <struct ZenitSymbolTable> and updates
 *  it -if needed- with type information the symbol could be missing.
 *  The function returns a <struct ZenitTypeInfo> with the type retrieved from the symbol
 *  or inferred from the context.
 */
typedef struct ZenitTypeInfo*(*ZenitTypeInferrer)(struct ZenitContext *ctx, struct ZenitNode *node);

// Visitor functions
static struct ZenitTypeInfo* visit_node(struct ZenitContext *ctx, struct ZenitNode *node);
static struct ZenitTypeInfo* visit_literal(struct ZenitContext *ctx, struct ZenitNode *node);
static struct ZenitTypeInfo* visit_variable(struct ZenitContext *ctx, struct ZenitNode *node);
static struct ZenitTypeInfo* visit_array_initializer(struct ZenitContext *ctx, struct ZenitNode *node);
static struct ZenitTypeInfo* visit_identifier(struct ZenitContext *ctx, struct ZenitNode *node);
static struct ZenitTypeInfo* visit_unary_ref(struct ZenitContext *ctx, struct ZenitNode *node);

/*
 * Variable: inferrers
 *  An array indexed with a <enum ZenitNodeType> to get a <ZenitTypeInferrer> function
 */
static const ZenitTypeInferrer inferrers[] = {
    [ZENIT_NODE_LITERAL]    = &visit_literal,
    [ZENIT_NODE_VARIABLE]   = &visit_variable,
    [ZENIT_NODE_ARRAY_INIT] = &visit_array_initializer,
    [ZENIT_NODE_IDENTIFIER] = &visit_identifier,
    [ZENIT_NODE_UNARY_REF]  = &visit_unary_ref,
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
 *  struct ZenitTypeInfo* - The intrinsic type of the literal node
 *
 */
static struct ZenitTypeInfo* visit_literal(struct ZenitContext *ctx, struct ZenitNode *node)
{
    return &((struct ZenitLiteralNode*)node)->base.typeinfo;
}

/*
 * Function: visit_identifier
 *  At this point the symbol must be defined, so we need to retrieve it from the symbol table
 *  and return its type information
 *
 * Parameters:
 *  ctx - Context object
 *  node - Literal node
 *
 * Returns:
 *  struct ZenitTypeInfo* - The type of the identifier
 *
 */
static struct ZenitTypeInfo* visit_identifier(struct ZenitContext *ctx, struct ZenitNode *node)
{
    return &zenit_program_get_symbol(ctx->program, ((struct ZenitIdentifierNode*)node)->name)->typeinfo;
}

/*
 * Function: visit_unary_ref
 *  In this function we need to fill the type information of the <struct ZenitUnaryRefNode> object using
 *  the expression typeinfo object.
 *
 * Parameters:
 *  ctx - Context object
 *  node - Array initializer node
 *
 * Returns:
 *  struct ZenitTypeInfo* - The inferred type of the array
 *
 */
static struct ZenitTypeInfo* visit_unary_ref(struct ZenitContext *ctx, struct ZenitNode *node)
{
    struct ZenitUnaryRefNode *ref_node = (struct ZenitUnaryRefNode*)node;
    struct ZenitTypeInfo *expr_info = visit_node(ctx, ref_node->expression);

    // Update the type of the unary ref node
    zenit_type_copy(&ref_node->base.typeinfo, expr_info);
    ref_node->base.typeinfo.is_ref = true;

    return &ref_node->base.typeinfo;
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
 *  struct ZenitTypeInfo* - The inferred type of the array
 *
 */
static struct ZenitTypeInfo* visit_array_initializer(struct ZenitContext *ctx, struct ZenitNode *node)
{
    struct ZenitArrayNode *array = (struct ZenitArrayNode*)node;

    size_t nelements = fl_array_length(array->elements);
    struct ZenitTypeInfo **elem_types = fl_array_new(sizeof(struct ZenitTypeInfo*), nelements);
    
    struct ZenitTypeInfo array_elements_common_type = { 0 };
    bool inference_failed = false;

    for (size_t i=0; i < nelements; i++)
    {
        struct ZenitTypeInfo *elem_type = visit_node(ctx, array->elements[i]);
        elem_types[i] = elem_type;

        if (!zenit_type_unify(&array_elements_common_type, elem_type))
        {
            inference_failed = true;
            zenit_context_error(ctx, array->elements[i]->location, ZENIT_ERROR_INFERENCE, 
                "Cannot find the common ancestor of types '%s' and '%s'", zenit_type_to_string(&array_elements_common_type), zenit_type_to_string(elem_type));
        }
    }

    if (!inference_failed)
    {
        for (size_t i=0; i < nelements; i++)
            zenit_type_unify(&array_elements_common_type, elem_types[i]);

        array->base.typeinfo.type = array_elements_common_type.type;
        array->base.typeinfo.name = array_elements_common_type.name;
        
        if (array_elements_common_type.is_ref)
            array->base.typeinfo.is_ref = true;
    }

    fl_array_free(elem_types);

    return &array->base.typeinfo;
}

/*
 * Function: visit_variable
 *  This function infers the variable type from the right-hand side expression
 *  or the other way around. If the type of both elements are <ZENIT_TYPE_NON> 
 *  this function adds a new error because that means we can't infer their types.
 *  If both types are defined bu are not equals, we don't need to do anything here,
 *  the type checking pass will take care of it.
 *
 * Parameters:
 *  ctx - Context object
 *  node - Variable declaration node
 *
 * Returns:
 *  struct ZenitTypeInfo* - The declared or inferred type of the variable
 *
 */
static struct ZenitTypeInfo* visit_variable(struct ZenitContext *ctx, struct ZenitNode *node)
{
    struct ZenitVariableNode *var_decl = (struct ZenitVariableNode*)node;
    
    // We need the symbol we introduced in the <zenit_resolve_symbols> pass
    struct ZenitSymbol *symbol = zenit_program_get_symbol(ctx->program, var_decl->name);

    // We need to get the type of the right-hand side expression
    struct ZenitTypeInfo *rhs_type = visit_node(ctx, var_decl->value);

    if (symbol->typeinfo.type == ZENIT_TYPE_NONE && rhs_type->type != ZENIT_TYPE_NONE)
    {
        // If symbol's type is none, we use the rhs type
        zenit_type_copy(&symbol->typeinfo, rhs_type);
    }
    else if (symbol->typeinfo.type != ZENIT_TYPE_NONE && rhs_type->type == ZENIT_TYPE_NONE)
    {
        // If the rhs type is none, we update it using the variable's type
        zenit_type_copy(rhs_type, &symbol->typeinfo);
    }
    else if (symbol->typeinfo.type == ZENIT_TYPE_NONE && rhs_type->type == ZENIT_TYPE_NONE)
    {
        // If both are none, it is an error for us
        zenit_context_error(ctx, var_decl->base.location, ZENIT_ERROR_INFERENCE, 
                "Cannot infer type of variable '%s' from the right-hand expression. Try making it explicit in the variable.", var_decl->name);
    }
    else
    {   
        // Try to unify, but ignore errors, the type check phase will take care of it
        //zenit_type_unify(&symbol->typeinfo, rhs_type);     
    }

    // FIXME: Visit the attributes

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
 *  struct ZenitTypeInfo* - A pointer to an object containing type information or NULL
 *
 */
static struct ZenitTypeInfo* visit_node(struct ZenitContext *ctx, struct ZenitNode *node)
{
    return inferrers[node->type](ctx, node);
}

/*
 * Function: zenit_infer_symbols
 *  We just iterate over the declarations visiting each node
 */
bool zenit_infer_types(struct ZenitContext *ctx)
{
    if (!ctx || !ctx->ast || !ctx->ast->decls)
        return false;

    size_t errors = zenit_context_error_count(ctx);

    for (size_t i=0; i < fl_array_length(ctx->ast->decls); i++)
        visit_node(ctx, ctx->ast->decls[i]);

    return errors == zenit_context_error_count(ctx);
}
