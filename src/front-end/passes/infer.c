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
static struct ZenitSymbol* visit_field_decl_node(struct ZenitContext *ctx, struct ZenitFieldDeclNode *field_node, struct ZenitTypeInfo *typehint);
static struct ZenitSymbol* visit_struct_decl_node(struct ZenitContext *ctx, struct ZenitStructDeclNode *struct_node, struct ZenitTypeInfo *typehint);

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
    [ZENIT_NODE_FIELD_DECL]      = (ZenitTypeInferrer) &visit_field_decl_node,
    [ZENIT_NODE_STRUCT_DECL]     = (ZenitTypeInferrer) &visit_struct_decl_node,
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

    if (!symbol_a->typeinfo->sealed)
    {
        if (!zenit_type_equals(symbol_a->typeinfo, unified))
            zenit_symbol_set_type(symbol_a, unified);
        result = ZENIT_TYPE_UNIFY_LEFT;
    }

    if (!symbol_b->typeinfo->sealed)
    {
        if (!zenit_type_equals(symbol_b->typeinfo, unified))
            zenit_symbol_set_type(symbol_b, unified);
        result = ZENIT_TYPE_UNIFY_RIGHT + result;
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

    if (!symbol->typeinfo->sealed)
    {
        if (!zenit_type_equals(symbol->typeinfo, unified))
            zenit_symbol_set_type(symbol, unified);
        result = ZENIT_TYPE_UNIFY_LEFT;
    }

    if (!(*typeinfo)->sealed)
    {
        if (!zenit_type_equals(*typeinfo, unified))
        {
            zenit_type_free(*typeinfo);
            *typeinfo = zenit_type_copy(unified);
        }
        result = ZENIT_TYPE_UNIFY_RIGHT + result;
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
            "Cannot infer the type of the cast expression, try adding a type hint to the cast expression"
        );
    }
    else if (typehint != NULL && zenit_type_can_unify(cast_symbol->typeinfo, typehint))
    {
        // FIXME: Should we check the result of this call?
        unify_symbol_type(cast_symbol, &typehint);
    }

    cast_symbol->typeinfo->sealed = true;

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
    // Get the reference symbol and its type
    struct ZenitSymbol *ref_symbol = zenit_utils_get_tmp_symbol(ctx->program, (struct ZenitNode*) reference_node);
    struct ZenitReferenceTypeInfo *ref_type = (struct ZenitReferenceTypeInfo*) ref_symbol->typeinfo;

    // Visit the expression node to get its symbol
    struct ZenitSymbol *expr_symbol = visit_node(ctx, reference_node->expression, NULL);

    // Try to unify the types
    enum ZenitTypeUnifyResult unify_result = ZENIT_TYPE_UNIFY_NONE;

    if (zenit_type_can_unify(expr_symbol->typeinfo, ref_type->element))
        unify_result = unify_symbol_type(expr_symbol, &ref_type->element);

    // If the unify result is NONE, and we can cast the element type to the array's type, we need to assume 
    // an implicit cast, and then the type check pass will take care of it
    if (unify_result == ZENIT_TYPE_UNIFY_NONE && zenit_type_is_castable_to(ref_type->element, expr_symbol->typeinfo))
    {
        // NOTE: The structure of the reference node changes, and so does its UID, because of that we need to update the
        // temporal symbol tied to the reference node (which is variable ref_symbol)

        // Create the cast node for the expression being referenced
        struct ZenitCastNode *cast_node = zenit_node_cast_new(reference_node->expression->location, reference_node->expression, true);

        // This call affects the reference node's structure and its UID
        reference_node->expression = (struct ZenitNode*) cast_node;

        // We add the temporal symbol for the cast node
        zenit_utils_new_tmp_symbol(ctx->program, (struct ZenitNode*) cast_node, zenit_type_copy(ref_type->element));

        // Finally, we update the symbol table with the updated node (with its new UID) and the -now- obsolete symbol name
        // (the old UID, which is the name of the temporal symbol)
        zenit_utils_get_update_symbol(ctx->program, (struct ZenitNode*) reference_node, ref_symbol->name);
    }

    ref_symbol->typeinfo->sealed = true;

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

    // If we received a type hint from the calling context, we can use it to update our array's member type,
    // and we can also use it with every element within the array
    // If the typehint is not null, at the end of this check, we will get a "member" type hint
    struct ZenitTypeInfo *member_type_hint = NULL;
    bool hint_needs_cleanup = false;
    if (typehint != NULL)
    {
        if (zenit_type_can_unify(array_symbol->typeinfo, typehint))
        {
            enum ZenitTypeUnifyResult unify_result = unify_symbol_type(array_symbol, &typehint);

            // Take the -probably- updated member type to use it as a hint for the elements to visit
            // NOTE: If typehint_used is false, something odd happened
            if (unify_result != ZENIT_TYPE_UNIFY_NONE)
                member_type_hint = ((struct ZenitArrayTypeInfo*) array_symbol->typeinfo)->member_type;
        }
        else if (typehint->type == ZENIT_TYPE_ARRAY)
        {
            // If we can't unify the types, but the type hint is an array too, we can use it's member type as a hint for the
            // elements to visit. Even if the type check pass rejects the operations later, this allows us to provide more context
            // for the elements.
            // E.G.: 
            //  "var a : [1]&uint8 = [ cast(0x1) ];" 
            //  In this case, &uint8 will be passed as a hint to the element "cast(0x1)", and the inference
            //  pass will interpret that as "cast(0x1 : &uint8)", even though the type check pass will reject
            //  the cast (cannot cast from uint8 to &uint8)
            // NOTE: The user provided the type hint, because of that we TRY to make use of it, but it can be an error too, because
            // of that we change the "source" of the type information to "inferred"
            member_type_hint = zenit_type_copy(((struct ZenitArrayTypeInfo*) typehint)->member_type);
            member_type_hint->source = ZENIT_TYPE_SRC_INFERRED;
            hint_needs_cleanup = true;
        }
    }    

    struct ZenitArrayTypeInfo *array_type = (struct ZenitArrayTypeInfo*) array_symbol->typeinfo;

    // Visit each element node and try to unify each element type with the array's member type
    for (size_t i=0; i < fl_array_length(array_node->elements); i++)
    {
        // If we received a type hint, and we used it to infer the array type, we can pass the member_type property as a hint to the
        // array element visitor
        struct ZenitSymbol *elem_symbol = visit_node(ctx, array_node->elements[i], member_type_hint);

        // Try to unify the types
        enum ZenitTypeUnifyResult unify_result = ZENIT_TYPE_UNIFY_NONE;

        if (zenit_type_can_unify(elem_symbol->typeinfo, array_type->member_type))
            unify_result = unify_symbol_type(elem_symbol, &array_type->member_type);

        // If the unify result is NONE, and we can cast the element type to the array's type, we need to assume 
        // an implicit cast, and then the type check pass will take care of it
        if (unify_result == ZENIT_TYPE_UNIFY_NONE && zenit_type_is_castable_to(array_type->member_type, elem_symbol->typeinfo))
        {
            // NOTE: the ZenitArrayNode structure changes, but we don't need to worry about its UID changing because the
            // elements of the array are ignored when calculating its UID
            struct ZenitCastNode *cast_node = zenit_node_cast_new(array_node->elements[i]->location, array_node->elements[i], true);
            array_node->elements[i] = (struct ZenitNode*) cast_node;

            zenit_utils_new_tmp_symbol(ctx->program, (struct ZenitNode*) cast_node, zenit_type_copy(array_type->member_type));
        }
    }

    if (hint_needs_cleanup)
        zenit_type_free(member_type_hint);

    array_symbol->typeinfo->sealed = true;
    
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

static struct ZenitSymbol* visit_field_decl_node(struct ZenitContext *ctx, struct ZenitFieldDeclNode *field_node, struct ZenitTypeInfo *typehint)
{
    return zenit_program_get_symbol(ctx->program, field_node->name);
}

static struct ZenitSymbol* visit_struct_decl_node(struct ZenitContext *ctx, struct ZenitStructDeclNode *struct_node, struct ZenitTypeInfo *typehint)
{
    // Visit the attributes and its properties
    visit_attribute_node_map(ctx, &struct_node->attributes);

    for (size_t i=0; i < fl_array_length(struct_node->members); i++)
        visit_node(ctx, struct_node->members[i], NULL);

    return NULL;
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
    // Visit the attributes and its properties
    visit_attribute_node_map(ctx, &variable_node->attributes);

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
            // NOTE: the ZenitVariableNode structure changes, but we don't need to worry about its UID changing because of that
            // as the variables are always accessed by its name, and not by the UID
            struct ZenitCastNode *cast_node = zenit_node_cast_new(variable_node->rvalue->location, variable_node->rvalue, true);
            variable_node->rvalue = (struct ZenitNode*) cast_node;

            zenit_utils_new_tmp_symbol(ctx->program, (struct ZenitNode*) cast_node, zenit_type_copy(symbol->typeinfo));
        }
    }

    symbol->typeinfo->sealed = true;

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
