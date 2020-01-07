#include "infer.h"
#include "utils.h"
#include "../program.h"
#include "../symbol.h"

/*
 * Type: ZenitTypeInferrer
 *  An inferrer function takes a symbol from the <struct ZenitSymtable> and updates
 *  it -if needed- with type information the symbol could be missing.
 *  The function returns a <struct ZenitTypeInfo> with the type retrieved from the symbol
 *  or inferred from the context.
 */
typedef struct ZenitSymbol*(*ZenitTypeInferrer)(struct ZenitContext *ctx, struct ZenitNode *node, struct ZenitTypeInfo *typehint);

// Visitor functions
static struct ZenitSymbol* visit_node(struct ZenitContext *ctx, struct ZenitNode *node, struct ZenitTypeInfo *typehint);
static struct ZenitSymbol* visit_primitive_node(struct ZenitContext *ctx, struct ZenitUintNode *node, struct ZenitTypeInfo *typehint);
static struct ZenitSymbol* visit_variable_node(struct ZenitContext *ctx, struct ZenitVariableNode *variable_node, struct ZenitTypeInfo *typehint);
static struct ZenitSymbol* visit_array_node(struct ZenitContext *ctx, struct ZenitArrayNode *array_node, struct ZenitTypeInfo *typehint);
static struct ZenitSymbol* visit_identifier_node(struct ZenitContext *ctx, struct ZenitIdentifierNode *node, struct ZenitTypeInfo *typehint);
static struct ZenitSymbol* visit_reference_node(struct ZenitContext *ctx, struct ZenitReferenceNode *reference_node, struct ZenitTypeInfo *typehint);
static struct ZenitSymbol* visit_cast_node(struct ZenitContext *ctx, struct ZenitCastNode *cast_node, struct ZenitTypeInfo *typehint);

/*
 * Variable: inferrers
 *  An array indexed with a <enum ZenitNodeType> to get a <ZenitTypeInferrer> function
 */
static const ZenitTypeInferrer inferrers[] = {
    [ZENIT_NODE_UINT]       = (ZenitTypeInferrer) &visit_primitive_node,
    [ZENIT_NODE_VARIABLE]   = (ZenitTypeInferrer) &visit_variable_node,
    [ZENIT_NODE_ARRAY]      = (ZenitTypeInferrer) &visit_array_node,
    [ZENIT_NODE_IDENTIFIER] = (ZenitTypeInferrer) &visit_identifier_node,
    [ZENIT_NODE_REFERENCE]  = (ZenitTypeInferrer) &visit_reference_node,
    [ZENIT_NODE_CAST]       = (ZenitTypeInferrer) &visit_cast_node,
};

enum ZenitTypeUnifyResult {
    ZENIT_TYPE_UNIFY_NONE,
    ZENIT_TYPE_UNIFY_LEFT,
    ZENIT_TYPE_UNIFY_RIGHT,
    ZENIT_TYPE_UNIFY_BOTH
};

static inline enum ZenitTypeUnifyResult unify_symbols_type(struct ZenitSymbol *symbol_a, struct ZenitSymbol *symbol_b)
{
    if (zenit_type_equals(symbol_a->typeinfo, symbol_b->typeinfo))
        return ZENIT_TYPE_UNIFY_BOTH;

    struct ZenitTypeInfo *unified = zenit_type_unify(symbol_a->typeinfo, symbol_b->typeinfo);

    // This function will update the type information of the symbols as long
    // as the type information they currently hold has its roots in the inference process,
    // because we can't change the type of a symbol with an intrinsic type information nor
    // doing that with a symbol that got its type from the hint the user provided in the source
    enum ZenitTypeUnifyResult result = ZENIT_TYPE_UNIFY_NONE;

    if (symbol_a->typeinfo->source == ZENIT_TYPE_SRC_INFERRED && !zenit_type_equals(symbol_a->typeinfo, unified))
    {
        zenit_symbol_set_type(symbol_a, unified);
        result++;
    }

    if (symbol_b->typeinfo->source == ZENIT_TYPE_SRC_INFERRED && !zenit_type_equals(symbol_b->typeinfo, unified))
    {
        zenit_symbol_set_type(symbol_b, unified);
        result++;
    }

    zenit_type_free(unified);

    return result;
}

static inline enum ZenitTypeUnifyResult unify_symbol_type(struct ZenitSymbol *symbol, struct ZenitTypeInfo **typeinfo)
{
    if (zenit_type_equals(symbol->typeinfo, *typeinfo))
        return ZENIT_TYPE_UNIFY_BOTH;

    struct ZenitTypeInfo *unified = zenit_type_unify(symbol->typeinfo, *typeinfo);

    // This function will update the type information of the symbol or the type object as long
    // as the type information they currently hold has its roots in the inference process,
    // because we can't change the type of a symbol with an intrinsic type information nor
    // doing that with a type hinted by the user
    enum ZenitTypeUnifyResult result = ZENIT_TYPE_UNIFY_NONE;

    if (symbol->typeinfo->source == ZENIT_TYPE_SRC_INFERRED && !zenit_type_equals(symbol->typeinfo, unified))
    {
        zenit_symbol_set_type(symbol, unified);
        result++;
    }

    if ((*typeinfo)->source == ZENIT_TYPE_SRC_INFERRED && !zenit_type_equals(*typeinfo, unified))
    {
        zenit_type_free(*typeinfo);
        *typeinfo = zenit_type_copy(unified);
        result++;
    }

    zenit_type_free(unified);

    return result;
}

/*
 * Function: visit_cast_node
 *  The cast expression does not infer anything, because first it honors the casted expression's type,
 *  and second, the cast's type is explicit "cast(0x1FF : uint8)" or inferred by another node (like a
 *  variable declaration: "var a : uint8 = cast(0x1FF)"  )
 * 
 * Parameters:
 *  ctx - Context object
 *  node - Cast node
 *
 * Returns:
 *  struct ZenitTypeInfo* - The intrinsic type information of the cast node
 *
 */
static struct ZenitSymbol* visit_cast_node(struct ZenitContext *ctx, struct ZenitCastNode *cast_node, struct ZenitTypeInfo *typehint)
{
    struct ZenitSymbol *cast_symbol = zenit_utils_get_tmp_symbol(ctx->program, (struct ZenitNode*) cast_node);
    struct ZenitSymbol *casted_expression = visit_node(ctx, cast_node->expression, NULL);

    // If there is no type hint from the context and the cast expression has no type hint
    // either, we can't infer the type
    if (typehint == NULL && cast_symbol->typeinfo->type == ZENIT_TYPE_NONE)
    {
        zenit_context_error(
            ctx, 
            cast_node->base.location, 
            ZENIT_ERROR_INFERENCE, 
            "Cannot infer the type of the cast expression, try adding a type hint"
        );
    }
    else if (typehint != NULL && zenit_type_can_unify(cast_symbol->typeinfo, typehint))
    {
        // FIXME: Should we check the result of this call?
        unify_symbol_type(cast_symbol, &typehint);
    }

    return cast_symbol;
}

/*
 * Function: visit_primitive_node
 *  A literal node always has a type therefore we just return it
 *
 * Parameters:
 *  ctx - Context object
 *  node - Literal node
 *
 * Returns:
 *  struct ZenitTypeInfo* - The intrinsic type information of the literal node
 *
 */
static struct ZenitSymbol* visit_primitive_node(struct ZenitContext *ctx, struct ZenitUintNode *node, struct ZenitTypeInfo *typehint)
{
    return zenit_utils_get_tmp_symbol(ctx->program, (struct ZenitNode*) node);
}

/*
 * Function: visit_identifier_node
 *  At this point the symbol must be defined, so we need to retrieve it from the symbol table
 *  and return its type information
 *
 * Parameters:
 *  ctx - Context object
 *  node - Identifier node
 *
 * Returns:
 *  struct ZenitTypeInfo* - The type information of the identifier
 *
 */
static struct ZenitSymbol* visit_identifier_node(struct ZenitContext *ctx, struct ZenitIdentifierNode *node, struct ZenitTypeInfo *typehint)
{
    return zenit_program_get_symbol(ctx->program, node->name);
}

/*
 * Function: visit_reference_node
 *  In this function we need to fill the type information of the <struct ZenitReferenceNode> object using
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
static struct ZenitSymbol* visit_reference_node(struct ZenitContext *ctx, struct ZenitReferenceNode *reference_node, struct ZenitTypeInfo *typehint)
{
    // Visit the expression node to get its symbol
    struct ZenitSymbol *expr_symbol = visit_node(ctx, reference_node->expression, NULL);

    // Get the reference symbol and its type
    struct ZenitSymbol *ref_symbol = zenit_utils_get_tmp_symbol(ctx->program, (struct ZenitNode*) reference_node);
    struct ZenitReferenceTypeInfo *ref_typeinfo = (struct ZenitReferenceTypeInfo*) ref_symbol->typeinfo;

    // If the types are equals, no need to do anything
    if (!zenit_type_equals(ref_typeinfo->element, expr_symbol->typeinfo))
    {
        // FIXME: By now this situation can't happen, as the reference type is equals
        // to the expression type, and the expression must be a valid symbol (no forward
        // references yet)
        if (!zenit_type_can_unify(ref_typeinfo->element, expr_symbol->typeinfo))
        {
            zenit_context_error(ctx, reference_node->base.location, ZENIT_ERROR_INFERENCE, 
                    "Cannot find the common ancestor of types '%s' and '%s'", 
                        zenit_type_to_string(ref_typeinfo->element), zenit_type_to_string(expr_symbol->typeinfo));
        }
        struct ZenitTypeInfo *tmp = ref_typeinfo->element;
        ref_typeinfo->element = zenit_type_unify(ref_typeinfo->element, expr_symbol->typeinfo);
        zenit_type_free(tmp);
    }

    return ref_symbol;
}

/*
 * Function: visit_array_node
 *  The array initializer always has type NONE before this pass, so we here update it
 *  to be the type of the first element within the array that has a type different from
 *  NONE.
 *  If we receive a type hint, we also use that to infer the array type.
 *
 * Parameters:
 *  ctx - Context object
 *  node - Array initializer node
 *
 * Returns:
 *  struct ZenitTypeInfo* - The inferred type of the array
 *
 */
static struct ZenitSymbol* visit_array_node(struct ZenitContext *ctx, struct ZenitArrayNode *array_node, struct ZenitTypeInfo *typehint)
{
    struct ZenitSymbol *array_symbol = zenit_utils_get_tmp_symbol(ctx->program, (struct ZenitNode*) array_node);

    bool typehint_used = false;
    if (typehint != NULL && zenit_type_can_unify(array_symbol->typeinfo, typehint))
    {
        // No need to check the unification result, the array type is NONE
        unify_symbol_type(array_symbol, &typehint);
        typehint_used = true;
    }

    struct ZenitArrayTypeInfo *array_type = (struct ZenitArrayTypeInfo*) array_symbol->typeinfo;
    array_type->length = fl_array_length(array_node->elements);

    for (size_t i=0; i < fl_array_length(array_node->elements); i++)
    {
        // If we received a type hint, and we used it to infer the array type, we can pass the member_type property as a hint
        struct ZenitSymbol *elem_symbol = visit_node(ctx, array_node->elements[i], typehint_used ? array_type->member_type : NULL);

        // Try to unify the types
        enum ZenitTypeUnifyResult unify_result = ZENIT_TYPE_UNIFY_NONE;

        if (zenit_type_can_unify(elem_symbol->typeinfo, array_type->member_type))
            unify_result = unify_symbol_type(elem_symbol, &array_type->member_type);

        // If the unify result is NONE, we need to assume an implicit cast, and then the type check pass will take care of it
        if (unify_result == ZENIT_TYPE_UNIFY_NONE && zenit_type_is_castable_to(array_type->member_type, elem_symbol->typeinfo))
        {
            struct ZenitCastNode *cast_node = zenit_node_cast_new(array_node->elements[i]->location, array_node->elements[i], true);
            array_node->elements[i] = (struct ZenitNode*) cast_node;

            zenit_utils_new_tmp_symbol(ctx->program, (struct ZenitNode*) cast_node, zenit_type_copy(elem_symbol->typeinfo));
        }

        zenit_type_free(array_type->members[i]);
        array_type->members[i] = zenit_type_copy(array_type->member_type);
    }
    
    return array_symbol;
}

/*
 * Function: visit_attribute_node_map
 *  We iterate over all the attributes to visit all the properties
 *
 * Parameters:
 *  <struct ZenitContext> *ctx - Context object
 *  <struct ZenitAttributeNodeMap> *attributes: Attributes map
 * 
 * Returns:
 *  void - This function does not return a value
 *
 * Notes:
 *  By now, this function is pretty dumb because there is nothing to infer in the
 *  properties
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

            struct ZenitSymbol *prop_symbol = zenit_utils_get_tmp_symbol(ctx->program, (struct ZenitNode*) prop);
            struct ZenitSymbol *value_symbol = visit_node(ctx, prop->value, NULL);

            // FIXME: Properties do not have types -so far-
        }

        fl_array_free(properties);
    }

    fl_array_free(names);
}

/*
 * Function: visit_variable_node
 *  This function infers the variable type from the right-hand side expression
 *  or the other way around. If the type of both elements are <ZENIT_TYPE_NONE> 
 *  this function adds a new error because that means we can't infer their types.
 *  If both types are defined but are not equals, we don't need to do anything here,
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
static struct ZenitSymbol* visit_variable_node(struct ZenitContext *ctx, struct ZenitVariableNode *variable_node, struct ZenitTypeInfo *typehint)
{
    // We need the symbol we introduced in the <zenit_resolve_symbols> pass
    struct ZenitSymbol *symbol = zenit_program_get_symbol(ctx->program, variable_node->name);

    // We need to get the symbol of the right-hand side expression. (if the variable definition has a type hint, we pass that hint to the visitor)
    struct ZenitSymbol *rhs_symbol = visit_node(ctx, variable_node->rvalue, symbol->typeinfo->source == ZENIT_TYPE_SRC_HINT ? symbol->typeinfo : NULL);

    // If the types are equals, we don't have to do anything here, but if they not, we need to
    // take an action about that
    if (!zenit_type_equals(symbol->typeinfo, rhs_symbol->typeinfo))
    {   
        // Try to unify the types
        enum ZenitTypeUnifyResult unify_result = ZENIT_TYPE_UNIFY_NONE;

        if (zenit_type_can_unify(symbol->typeinfo, rhs_symbol->typeinfo))
            unify_result = unify_symbols_type(symbol, rhs_symbol);


        // If the unify result is NONE, we need to assume an implicit cast, and then the type check pass will take care of it
        if (unify_result == ZENIT_TYPE_UNIFY_NONE && zenit_type_is_castable_to(rhs_symbol->typeinfo, symbol->typeinfo))
        {
            struct ZenitCastNode *cast_node = zenit_node_cast_new(variable_node->rvalue->location, variable_node->rvalue, true);
            variable_node->rvalue = (struct ZenitNode*) cast_node;

            zenit_utils_new_tmp_symbol(ctx->program, (struct ZenitNode*) cast_node, zenit_type_copy(symbol->typeinfo));
        }
    }

    // Visit the attributes and its properties
    visit_attribute_node_map(ctx, &variable_node->attributes);

    // We always return the variable symbol
    return symbol;
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
static struct ZenitSymbol* visit_node(struct ZenitContext *ctx, struct ZenitNode *node, struct ZenitTypeInfo *typehint)
{
    return inferrers[node->type](ctx, node, typehint);
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
        visit_node(ctx, ctx->ast->decls[i], NULL);

    return errors == zenit_context_error_count(ctx);
}
