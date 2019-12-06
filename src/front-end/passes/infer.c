#include "infer.h"
#include "../program.h"
#include "../symbol.h"

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
static struct ZenitTypeInfo* visit_primitive_node(struct ZenitContext *ctx, struct ZenitPrimitiveNode *node);
static struct ZenitTypeInfo* visit_variable_node(struct ZenitContext *ctx, struct ZenitVariableNode *node);
static struct ZenitTypeInfo* visit_array_node(struct ZenitContext *ctx, struct ZenitArrayNode *node);
static struct ZenitTypeInfo* visit_identifier_node(struct ZenitContext *ctx, struct ZenitIdentifierNode *node);
static struct ZenitTypeInfo* visit_reference_node(struct ZenitContext *ctx, struct ZenitReferenceNode *node);
static struct ZenitTypeInfo* visit_cast_node(struct ZenitContext *ctx, struct ZenitCastNode *node);

/*
 * Variable: inferrers
 *  An array indexed with a <enum ZenitNodeType> to get a <ZenitTypeInferrer> function
 */
static const ZenitTypeInferrer inferrers[] = {
    [ZENIT_NODE_LITERAL]    = (ZenitTypeInferrer) &visit_primitive_node,
    [ZENIT_NODE_VARIABLE]   = (ZenitTypeInferrer) &visit_variable_node,
    [ZENIT_NODE_ARRAY]      = (ZenitTypeInferrer) &visit_array_node,
    [ZENIT_NODE_IDENTIFIER] = (ZenitTypeInferrer) &visit_identifier_node,
    [ZENIT_NODE_REFERENCE]  = (ZenitTypeInferrer) &visit_reference_node,
    [ZENIT_NODE_CAST]       = (ZenitTypeInferrer) &visit_cast_node,
};

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
static struct ZenitTypeInfo* visit_cast_node(struct ZenitContext *ctx, struct ZenitCastNode *cast_node)
{
    visit_node(ctx, cast_node->expression);

    // We always return the cast's type, if it is NONE, we will need information
    // from the context, and if that information is not present, we will error out
    // in the type check pass
    return &cast_node->base.typeinfo;
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
static struct ZenitTypeInfo* visit_primitive_node(struct ZenitContext *ctx, struct ZenitPrimitiveNode *node)
{
    return &node->base.typeinfo;
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
static struct ZenitTypeInfo* visit_identifier_node(struct ZenitContext *ctx, struct ZenitIdentifierNode *node)
{
    return &zenit_program_get_symbol(ctx->program, node->name)->typeinfo;
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
static struct ZenitTypeInfo* visit_reference_node(struct ZenitContext *ctx, struct ZenitReferenceNode *reference_node)
{
    struct ZenitTypeInfo *expression_typeinfo = visit_node(ctx, reference_node->expression);

    // Update the type of the unary ref node
    zenit_type_copy(&reference_node->base.typeinfo, expression_typeinfo);
    reference_node->base.typeinfo.is_ref = true;

    return &reference_node->base.typeinfo;
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
static struct ZenitTypeInfo* visit_array_node(struct ZenitContext *ctx, struct ZenitArrayNode *array)
{
    size_t length = fl_array_length(array->elements);

    struct ZenitTypeInfo **elements_type = fl_array_new(sizeof(struct ZenitTypeInfo*), length);
    
    struct ZenitTypeInfo common_type = { 0 };
    bool inference_failed = false;

    for (size_t i=0; i < length; i++)
    {
        struct ZenitTypeInfo *elem_type = visit_node(ctx, array->elements[i]);
        elements_type[i] = elem_type;

        if (!zenit_type_unify(&common_type, elem_type))
        {
            inference_failed = true;
            zenit_context_error(ctx, array->elements[i]->location, ZENIT_ERROR_INFERENCE, 
                "Cannot find the common ancestor of types '%s' and '%s'", zenit_type_to_string(&common_type), zenit_type_to_string(elem_type));
        }
    }

    if (!inference_failed)
    {
        // FIXME: Even though this work for uints it probably breaks
        // for complex types (what does happen if common_type changes here??)
        for (size_t i=0; i < length; i++)
            zenit_type_unify(&common_type, elements_type[i]);

        array->base.typeinfo.type = common_type.type;
        array->base.typeinfo.name = common_type.name;
        
        if (common_type.is_ref)
            array->base.typeinfo.is_ref = true;
    }

    fl_array_free(elements_type);

    return &array->base.typeinfo;
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
            struct ZenitPropertyNode *prop = properties[i];
            visit_node(ctx, prop->value);
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
static struct ZenitTypeInfo* visit_variable_node(struct ZenitContext *ctx, struct ZenitVariableNode *variable_node)
{
    // We need the symbol we introduced in the <zenit_resolve_symbols> pass
    struct ZenitSymbol *symbol = zenit_program_get_symbol(ctx->program, variable_node->name);

    // We need to get the type of the right-hand side expression
    struct ZenitTypeInfo *rhs_type = visit_node(ctx, variable_node->rvalue);

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
        zenit_context_error(ctx, variable_node->base.location, ZENIT_ERROR_INFERENCE, 
                "Cannot infer type of variable '%s' from the right-hand expression. Try making the type explicit in the variable declaration.", variable_node->name);
    }
    else if (!zenit_type_equals(&symbol->typeinfo, rhs_type))
    {   
        // Last resort: Both types are defined, so we assume an implicit cast, the type check pass will take care of it
        variable_node->rvalue = (struct ZenitNode*)zenit_node_cast_new(variable_node->rvalue->location, variable_node->rvalue, &variable_node->base.typeinfo, true);
    }

    // Visit the attributes and its properties
    visit_attribute_node_map(ctx, &variable_node->attributes);

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
