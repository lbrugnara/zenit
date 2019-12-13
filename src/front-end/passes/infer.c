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
    [ZENIT_NODE_UINT]    = (ZenitTypeInferrer) &visit_primitive_node,
    [ZENIT_NODE_VARIABLE]   = (ZenitTypeInferrer) &visit_variable_node,
    [ZENIT_NODE_ARRAY]      = (ZenitTypeInferrer) &visit_array_node,
    [ZENIT_NODE_IDENTIFIER] = (ZenitTypeInferrer) &visit_identifier_node,
    [ZENIT_NODE_REFERENCE]  = (ZenitTypeInferrer) &visit_reference_node,
    [ZENIT_NODE_CAST]       = (ZenitTypeInferrer) &visit_cast_node,
};

static inline void unify_symbols_type(struct ZenitSymbol *symbol_a, struct ZenitSymbol *symbol_b)
{
    if (zenit_type_equals(symbol_a->typeinfo, symbol_b->typeinfo))
        return;

    struct ZenitTypeInfo *unified = zenit_type_unify(symbol_a->typeinfo, symbol_b->typeinfo);

    // This function will update the type information of the symbols as long
    // as the type information they currently hold has its roots in the inference process,
    // because we can't change the type of a symbol with an intrinsic type information nor
    // doing that with a symbol that got its type from the hint the user provided in the source

    if (symbol_a->typeinfo->source == ZENIT_TYPE_SRC_INFERRED && !zenit_type_equals(symbol_a->typeinfo, unified))
    {
        zenit_type_free(symbol_a->typeinfo);
        symbol_a->typeinfo = zenit_type_copy(unified);
    }

    if (symbol_b->typeinfo->source == ZENIT_TYPE_SRC_INFERRED && !zenit_type_equals(symbol_b->typeinfo, unified))
    {
        zenit_type_free(symbol_b->typeinfo);
        symbol_b->typeinfo = zenit_type_copy(unified);
    }

    zenit_type_free(unified);
}

static inline void unify_symbol_type(struct ZenitSymbol *symbol, struct ZenitTypeInfo **typeinfo)
{
    if (zenit_type_equals(symbol->typeinfo, *typeinfo))
        return;

    struct ZenitTypeInfo *unified = zenit_type_unify(symbol->typeinfo, *typeinfo);

    // This function will update the type information of the symbol or the type object as long
    // as the type information they currently hold has its roots in the inference process,
    // because we can't change the type of a symbol with an intrinsic type information nor
    // doing that with a type hinted by the user

    if (symbol->typeinfo->source == ZENIT_TYPE_SRC_INFERRED && !zenit_type_equals(symbol->typeinfo, unified))
    {
        zenit_type_free(symbol->typeinfo);
        symbol->typeinfo = zenit_type_copy(unified);
    }

    if ((*typeinfo)->source == ZENIT_TYPE_SRC_INFERRED && !zenit_type_equals(*typeinfo, unified))
    {
        zenit_type_free(*typeinfo);
        *typeinfo = zenit_type_copy(unified);
    }

    zenit_type_free(unified);
}

static inline void unify_types(struct ZenitTypeInfo **typeinfo_a, struct ZenitTypeInfo **typeinfo_b)
{
    if (zenit_type_equals(*typeinfo_a, *typeinfo_b))
        return;

    struct ZenitTypeInfo *unified = zenit_type_unify(*typeinfo_a, *typeinfo_b);

    // This function will update the type information of the type objects as long
    // as the type information they currently hold has its roots in the inference process,
    // because we can't change a type with an intrinsic type information nor
    // doing that with a type hinted by the user

    if ((*typeinfo_a)->source == ZENIT_TYPE_SRC_INFERRED && !zenit_type_equals(*typeinfo_a, unified))
    {
        zenit_type_free(*typeinfo_a);
        *typeinfo_a = zenit_type_copy(unified);
    }

    if ((*typeinfo_b)->source == ZENIT_TYPE_SRC_INFERRED && !zenit_type_equals(*typeinfo_b, unified))
    {
        zenit_type_free(*typeinfo_b);
        *typeinfo_b = zenit_type_copy(unified);
    }

    zenit_type_free(unified);
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

    if (cast_node->implicit && !zenit_type_is_castable_to(casted_expression->typeinfo, cast_symbol->typeinfo))
    {
        if (zenit_type_can_unify(cast_symbol->typeinfo, casted_expression->typeinfo))
            unify_symbols_type(cast_symbol, casted_expression);
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
    struct ZenitSymbol *expr_symbol = visit_node(ctx, reference_node->expression, NULL);
    struct ZenitSymbol *ref_symbol = zenit_utils_get_tmp_symbol(ctx->program, (struct ZenitNode*) reference_node);
    struct ZenitReferenceTypeInfo *ref_typeinfo = (struct ZenitReferenceTypeInfo*) ref_symbol->typeinfo;

    // If the types are equals, no need to do anything
    if (!zenit_type_equals(ref_typeinfo->element, expr_symbol->typeinfo))
    {
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

    if (typehint != NULL && zenit_type_can_unify(array_symbol->typeinfo, typehint))
    {
        unify_symbol_type(array_symbol, &typehint);
    }

    struct ZenitArrayTypeInfo *array_type = (struct ZenitArrayTypeInfo*) array_symbol->typeinfo;

    //struct ZenitSymbol **array_elements_symbols = fl_array_new(sizeof(struct ZenitSymbol*), length);

    struct ZenitTypeInfo *common_type = zenit_type_copy(array_type->member_type);

    for (size_t i=0; i < fl_array_length(array_node->elements); i++)
    {
        struct ZenitSymbol *elem_symbol = visit_node(ctx, array_node->elements[i], NULL);

        //array_elements_symbols[i] = elem_symbol;

        if (common_type == NULL)
        {
            common_type = zenit_type_copy(elem_symbol->typeinfo);
            continue;
        }

        if (zenit_type_can_unify(elem_symbol->typeinfo, common_type))
        {
            unify_symbol_type(elem_symbol, &common_type);
        }
        else
        {
            zenit_context_error(ctx, array_node->elements[i]->location, ZENIT_ERROR_INFERENCE, 
                    "Cannot find the common ancestor of types '%s' and '%s'", 
                        zenit_type_to_string(common_type), zenit_type_to_string(elem_symbol->typeinfo));
        }
    }

    for (size_t i=0; i < fl_array_length(array_node->elements); i++)
    {
        zenit_type_free(array_type->members[i]);
        array_type->members[i] = zenit_type_copy(common_type);
        //zenit_type_free(array_elements_symbols[i]->typeinfo);
        //array_elements_symbols[i]->typeinfo = zenit_type_copy(common_type);
        //zenit_type_array_add_member(array_type, zenit_type_copy(common_type));
    }
    
    zenit_type_free(array_type->member_type);
    array_type->member_type = zenit_type_copy(common_type);
    zenit_type_free(common_type);
    array_type->length = fl_array_length(array_node->elements);

    //fl_array_free(array_elements_symbols);

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
static struct ZenitSymbol* visit_variable_node(struct ZenitContext *ctx, struct ZenitVariableNode *variable_node, struct ZenitTypeInfo *typehint)
{
    // We need the symbol we introduced in the <zenit_resolve_symbols> pass
    struct ZenitSymbol *symbol = zenit_program_get_symbol(ctx->program, variable_node->name);

    // We need to get the type of the right-hand side expression
    struct ZenitSymbol *rhs_symbol = visit_node(ctx, variable_node->rvalue, symbol->typeinfo->source == ZENIT_TYPE_SRC_HINT ? symbol->typeinfo : NULL);

    // If the types are equals, we don't have to do anything here, but if not, we need to
    // take action about that
    if (!zenit_type_equals(symbol->typeinfo, rhs_symbol->typeinfo))
    {   
        // If the types can be "unified", that means, finding a common ancestor between
        // them, we can try that
        if (zenit_type_can_unify(symbol->typeinfo, rhs_symbol->typeinfo))
        {
            unify_symbols_type(symbol, rhs_symbol);
        }
        else if (zenit_type_is_castable_to(rhs_symbol->typeinfo, symbol->typeinfo))
        {
            // Last resort: We assume an implicit cast, the type check pass will take care of it
            struct ZenitCastNode *cast_node = zenit_node_cast_new(variable_node->rvalue->location, variable_node->rvalue, true);
            variable_node->rvalue = (struct ZenitNode*) cast_node;

            zenit_utils_new_tmp_symbol(ctx->program, (struct ZenitNode*) cast_node, zenit_type_copy(symbol->typeinfo));
        }
    }

    // Visit the attributes and its properties
    visit_attribute_node_map(ctx, &variable_node->attributes);

    // Always the declared or inferred type. In case of error, we honor the variable type
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
