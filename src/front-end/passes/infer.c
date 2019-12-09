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
typedef struct ZenitSymbol*(*ZenitTypeInferrer)(struct ZenitContext *ctx, struct ZenitNode *node);

// Visitor functions
static struct ZenitSymbol* visit_node(struct ZenitContext *ctx, struct ZenitNode *node);
static struct ZenitSymbol* visit_primitive_node(struct ZenitContext *ctx, struct ZenitPrimitiveNode *node);
static struct ZenitSymbol* visit_variable_node(struct ZenitContext *ctx, struct ZenitVariableNode *node);
static struct ZenitSymbol* visit_array_node(struct ZenitContext *ctx, struct ZenitArrayNode *node);
static struct ZenitSymbol* visit_identifier_node(struct ZenitContext *ctx, struct ZenitIdentifierNode *node);
static struct ZenitSymbol* visit_reference_node(struct ZenitContext *ctx, struct ZenitReferenceNode *node);
static struct ZenitSymbol* visit_cast_node(struct ZenitContext *ctx, struct ZenitCastNode *node);

/*
 * Variable: inferrers
 *  An array indexed with a <enum ZenitNodeType> to get a <ZenitTypeInferrer> function
 */
static const ZenitTypeInferrer inferrers[] = {
    [ZENIT_NODE_PRIMITIVE]    = (ZenitTypeInferrer) &visit_primitive_node,
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
static struct ZenitSymbol* visit_cast_node(struct ZenitContext *ctx, struct ZenitCastNode *cast_node)
{
    visit_node(ctx, cast_node->expression);

    // We always return the cast's type, if it is NONE, we will need information
    // from the context, and if that information is not present, we will error out
    // in the type check pass
    return zenit_utils_get_tmp_symbol(ctx->program, (struct ZenitNode*) cast_node);
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
static struct ZenitSymbol* visit_primitive_node(struct ZenitContext *ctx, struct ZenitPrimitiveNode *node)
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
static struct ZenitSymbol* visit_identifier_node(struct ZenitContext *ctx, struct ZenitIdentifierNode *node)
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
static struct ZenitSymbol* visit_reference_node(struct ZenitContext *ctx, struct ZenitReferenceNode *reference_node)
{
    visit_node(ctx, reference_node->expression);

    struct ZenitSymbol *ref_symbol = zenit_utils_get_tmp_symbol(ctx->program, (struct ZenitNode*) reference_node);

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
static struct ZenitSymbol* visit_array_node(struct ZenitContext *ctx, struct ZenitArrayNode *array_node)
{
    struct ZenitSymbol *array_symbol = zenit_utils_get_tmp_symbol(ctx->program, (struct ZenitNode*) array_node);
    struct ZenitArrayTypeInfo *array_type = (struct ZenitArrayTypeInfo*) array_symbol->typeinfo;

    size_t length = fl_array_length(array_node->elements);

    struct ZenitSymbol **array_elements_symbols = fl_array_new(sizeof(struct ZenitSymbol*), length);

    struct ZenitTypeInfo *common_type = array_type->member_type;

    for (size_t i=0; i < length; i++)
    {
        struct ZenitSymbol *elem_symbol = visit_node(ctx, array_node->elements[i]);

        array_elements_symbols[i] = elem_symbol;

        if (common_type == NULL)
        {
            common_type = elem_symbol->typeinfo;
            continue;
        }

        struct ZenitTypeInfo *tmp = zenit_type_unify(common_type, elem_symbol->typeinfo);

        if (tmp == NULL)
        {
            zenit_context_error(ctx, array_node->elements[i]->location, ZENIT_ERROR_INFERENCE, 
                    "Cannot find the common ancestor of types '%s' and '%s'", 
                        zenit_type_to_string(common_type), zenit_type_to_string(elem_symbol->typeinfo));
            continue;
        }

        common_type = tmp;
    }

    for (size_t i=0; i < length; i++)
    {
        zenit_symbol_set_type(array_elements_symbols[i], common_type);
        zenit_type_array_add_member(array_type, common_type);
    }

    array_type->source = ZENIT_ARRAY_TYPE_INFER;
    array_type->member_type = common_type;
    array_type->length = length;

    fl_array_free(array_elements_symbols);

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


            struct ZenitSymbol *value_symbol = visit_node(ctx, prop->value);

            // FIXME: Properties do not have types -so far-, so it is safe to infer the type
            // from the value
            if (value_symbol->typeinfo->type == ZENIT_TYPE_NONE)
            {
                zenit_context_error(ctx, prop->base.location, ZENIT_ERROR_INFERENCE, 
                "Cannot infer type of property '%s' from its value.", prop->name);
            }
            else
            {
                zenit_symbol_set_type(prop_symbol, value_symbol->typeinfo);
            }
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
static struct ZenitSymbol* visit_variable_node(struct ZenitContext *ctx, struct ZenitVariableNode *variable_node)
{
    // We need the symbol we introduced in the <zenit_resolve_symbols> pass
    struct ZenitSymbol *symbol = zenit_program_get_symbol(ctx->program, variable_node->name);

    // We need to get the type of the right-hand side expression
    struct ZenitSymbol *rhs_symbol = visit_node(ctx, variable_node->rvalue);

    if (symbol->typeinfo->type == ZENIT_TYPE_NONE && rhs_symbol->typeinfo->type != ZENIT_TYPE_NONE)
    {
        // If symbol's type is none, we use the rhs type
        zenit_symbol_set_type(symbol, rhs_symbol->typeinfo);
    }
    else if (symbol->typeinfo->type != ZENIT_TYPE_NONE && rhs_symbol->typeinfo->type == ZENIT_TYPE_NONE)
    {
        // If the rhs type is none, we update it using the variable's type
        zenit_symbol_set_type(rhs_symbol, symbol->typeinfo);
    }
    else if (symbol->typeinfo->type == ZENIT_TYPE_NONE && rhs_symbol->typeinfo->type == ZENIT_TYPE_NONE)
    {
        // If both are none, it is an error for us
        zenit_context_error(ctx, variable_node->base.location, ZENIT_ERROR_INFERENCE, 
                "Cannot infer type of variable '%s' from the right-hand expression. Try making the type explicit in the variable declaration.", variable_node->name);
    }
    else if (!zenit_type_equals(symbol->typeinfo, rhs_symbol->typeinfo))
    {   
        // Last resort: Both types are defined, so we assume an implicit cast, the type check pass will take care of it
        struct ZenitCastNode *cast_node = zenit_node_cast_new(variable_node->rvalue->location, variable_node->rvalue, true);
        variable_node->rvalue = (struct ZenitNode*) cast_node;

        struct ZenitSymbol *cast_symbol = zenit_utils_new_tmp_symbol(ctx->program, (struct ZenitNode*) cast_node, symbol->typeinfo);

        if (cast_symbol == NULL)
        {
            zenit_context_error(ctx, variable_node->base.location, ZENIT_ERROR_INFERENCE, 
                "Cannot infer type of variable '%s' from the right-hand expression. Try making the type explicit in the variable declaration.", variable_node->name);
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
static struct ZenitSymbol* visit_node(struct ZenitContext *ctx, struct ZenitNode *node)
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
