#include "check.h"
#include "utils.h"
#include "../program.h"
#include "../symbol.h"

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
 *  bool - *true* if the type is a native type or a struct type defined in the symbol
 *          table, otherwise this function returns *false*.
 */
static bool is_type_defined(struct ZenitProgram *program, struct ZenitTypeInfo *typeinfo)
{
    if (typeinfo == NULL)
        return false;

    if (typeinfo->type->typekind == ZENIT_TYPE_UINT)
        return true;

    if (typeinfo->type->typekind == ZENIT_TYPE_REFERENCE)
    {
        struct ZenitReferenceType *rti = (struct ZenitReferenceType*) typeinfo->type;
        return is_type_defined(program, rti->element);
    }

    if (typeinfo->type->typekind == ZENIT_TYPE_STRUCT)
    {
        // FIXME: Update this once struct members are implemented
        return zenit_program_has_scope(program, ZENIT_SCOPE_STRUCT, ((struct ZenitStructType*) typeinfo->type)->name);
    }
    
    if (typeinfo->type->typekind == ZENIT_TYPE_ARRAY)
    {
        struct ZenitArrayType *ati = (struct ZenitArrayType*) typeinfo->type;
/*
        if (ati->decl_type && !is_type_defined(program, ati->decl_type))
            return false;

        if (ati->inferred_type && !is_type_defined(program, ati->inferred_type))
            return false;

        for (size_t i=0; i < fl_array_length(ati->members); i++)
            if (!is_type_defined(program, ati->members[i]))
                return false;
*/
        return is_type_defined(program, ati->member_type);
    }

    return false;
}

static struct ZenitTypeInfo* get_undefined_type(struct ZenitProgram *program, struct ZenitTypeInfo *typeinfo)
{
    // Cannot be undefined
    if (typeinfo->type->typekind == ZENIT_TYPE_UINT)
        return NULL;

    if (typeinfo->type->typekind == ZENIT_TYPE_REFERENCE)
    {
        struct ZenitReferenceType *rti = (struct ZenitReferenceType*) typeinfo->type;
        if (!is_type_defined(program, rti->element))
            return get_undefined_type(program, rti->element);

        return NULL;
    }

    if (typeinfo->type->typekind == ZENIT_TYPE_STRUCT)
    {
        // FIXME: Update this once struct members are implemented
        if (!zenit_program_has_symbol(program, ((struct ZenitStructType*) typeinfo->type)->name))
            return typeinfo;

        return NULL;
    }
    
    if (typeinfo->type->typekind == ZENIT_TYPE_ARRAY)
    {
        struct ZenitArrayType *ati = (struct ZenitArrayType*) typeinfo->type;

        /*if (ati->decl_type && !is_type_defined(program, ati->decl_type))
            return get_undefined_type(program, ati->decl_type);

        if (ati->inferred_type && !is_type_defined(program, ati->inferred_type))
            return get_undefined_type(program, ati->inferred_type);

        for (size_t i=0; i < fl_array_length(ati->members); i++)
            if (!is_type_defined(program, ati->members[i]))
                return get_undefined_type(program, ati->members[i]);*/

        return get_undefined_type(program, ati->member_type);
    }

    return NULL;
}

/*
 * Type: ZenitTypeChecker
 *  A checker function takes a symbol from the <struct ZenitSymtable> and 
 *  and makes sure its type is valid in the context that is determined
 *  by the <struct ZenitNode>.
 *  The function returns a <struct ZenitTypeInfo> pointer with the checked type information
 *  or NULL on error where the caller should know how to handle that and should keep
 *  propagating the error.
 */
typedef struct ZenitSymbol*(*ZenitTypeChecker)(struct ZenitContext *ctx, struct ZenitNode *node);

// Visitor functions
static struct ZenitSymbol* visit_node(struct ZenitContext *ctx, struct ZenitNode *node);
static struct ZenitSymbol* visit_primitive_node(struct ZenitContext *ctx, struct ZenitUintNode *node);
static struct ZenitSymbol* visit_variable_node(struct ZenitContext *ctx, struct ZenitVariableNode *node);
static struct ZenitSymbol* visit_array_node(struct ZenitContext *ctx, struct ZenitArrayNode *node);
static struct ZenitSymbol* visit_identifier_node(struct ZenitContext *ctx, struct ZenitIdentifierNode *node);
static struct ZenitSymbol* visit_reference_node(struct ZenitContext *ctx, struct ZenitReferenceNode *node);
static struct ZenitSymbol* visit_cast_node(struct ZenitContext *ctx, struct ZenitCastNode *node);
static struct ZenitSymbol* visit_field_decl_node(struct ZenitContext *ctx, struct ZenitFieldDeclNode *field_node);
static struct ZenitSymbol* visit_struct_decl_node(struct ZenitContext *ctx, struct ZenitStructDeclNode *struct_node);
static struct ZenitSymbol* visit_struct_node(struct ZenitContext *ctx, struct ZenitStructNode *struct_node);

/*
 * Variable: checkers
 *  An array indexed with a <enum ZenitNodeType> to get a <ZenitTypeChecker> function
 */
static const ZenitTypeChecker checkers[] = {
    [ZENIT_NODE_UINT]           = (ZenitTypeChecker) &visit_primitive_node,
    [ZENIT_NODE_VARIABLE]       = (ZenitTypeChecker) &visit_variable_node,
    [ZENIT_NODE_ARRAY]          = (ZenitTypeChecker) &visit_array_node,
    [ZENIT_NODE_IDENTIFIER]     = (ZenitTypeChecker) &visit_identifier_node,
    [ZENIT_NODE_REFERENCE]      = (ZenitTypeChecker) &visit_reference_node,
    [ZENIT_NODE_CAST]           = (ZenitTypeChecker) &visit_cast_node,
    [ZENIT_NODE_FIELD_DECL]     = (ZenitTypeChecker) &visit_field_decl_node,
    [ZENIT_NODE_STRUCT_DECL]    = (ZenitTypeChecker) &visit_struct_decl_node,
    [ZENIT_NODE_STRUCT]         = (ZenitTypeChecker) &visit_struct_node,
};

/*
 * Function: visit_cast_node
 *  We need to check if the cast (implicit or explicit) is valid for these expressions
 *
 * Parameters:
 *  ctx - Context object
 *  node - Node to visit
 *
 * Returns:
 *  struct ZenitSymbol* - The casted expression's type information
 */
static struct ZenitSymbol* visit_cast_node(struct ZenitContext *ctx, struct ZenitCastNode *cast)
{
    struct ZenitSymbol *symbol = zenit_utils_get_tmp_symbol(ctx->program, (struct ZenitNode*) cast);

    struct ZenitSymbol *expr_symbol = visit_node(ctx, cast->expression);

    // If the cast is implicit, we need to make sure we can up-cast the expression to the requested type
    if ((cast->implicit && !zenit_type_is_assignable_from(symbol->typeinfo->type, expr_symbol->typeinfo->type))
        // HACK: If the cast is explicit, we can check if "the other way assignment" around works for these types, 
        // in that, case we know it is safe to "truncate" the type :grimming:
        || (!cast->implicit && !zenit_type_is_castable_to(expr_symbol->typeinfo->type, symbol->typeinfo->type))
    )
    {
        zenit_context_error(ctx, cast->base.location, ZENIT_ERROR_TYPE_MISSMATCH, "Cannot %s from type '%s' to '%s'", 
                cast->implicit ? "implicitly cast" : "cast", 
                zenit_type_to_string(expr_symbol->typeinfo->type),
                zenit_type_to_string(symbol->typeinfo->type)
        );
    }

    return symbol;
}

/*
 * Function: visit_primitive_node
 *  The primitive node doesn't need to check anything, it just
 *  returns its type information
 *
 * Parameters:
 *  ctx - Context object
 *  node - Node to visit
 *
 * Returns:
 *  struct ZenitSymbol* - The primitive's type information
 */
static struct ZenitSymbol* visit_primitive_node(struct ZenitContext *ctx, struct ZenitUintNode *primitive)
{
    return zenit_utils_get_tmp_symbol(ctx->program, (struct ZenitNode*) primitive);
}

/*
 * Function: visit_identifier_node
 *  It just returns the type information of the identifier
 *
 * Parameters:
 *  ctx - Context object
 *  node - Node to visit
 *
 * Returns:
 *  struct ZenitSymbol* - The identifier type information
 */
static struct ZenitSymbol* visit_identifier_node(struct ZenitContext *ctx, struct ZenitIdentifierNode *id_node)
{
    return zenit_program_get_symbol(ctx->program, id_node->name);
}

/*
 * Function: visit_reference_node
 *  Check the referenced expression is a valid expression to take its address from
 *
 * Parameters:
 *  ctx - Context object
 *  node - Node to visit
 *
 * Returns:
 *  struct ZenitSymbol* - The reference type information
 */
static struct ZenitSymbol* visit_reference_node(struct ZenitContext *ctx, struct ZenitReferenceNode *reference_node)
{
    struct ZenitSymbol *ref_symbol = zenit_utils_get_tmp_symbol(ctx->program, (struct ZenitNode*) reference_node);
    struct ZenitReferenceType *ref_type = (struct ZenitReferenceType*) ref_symbol->typeinfo->type;

    struct ZenitSymbol *expression_symbol = visit_node(ctx, reference_node->expression);

    // FIXME: Cannot take a reference to a temporal symbol (temporal expression, primitive, etc)
    if (expression_symbol->name[0] == '%' && expression_symbol->typeinfo->type->typekind == ZENIT_TYPE_REFERENCE)
    {
        zenit_context_error(ctx, reference_node->base.location, ZENIT_ERROR_INVALID_REFERENCE, 
                "Cannot take a reference to another reference.");
    }

    if (!zenit_type_is_assignable_from(ref_type->element->type, expression_symbol->typeinfo->type))
    {
        zenit_context_error(ctx, reference_node->base.location, ZENIT_ERROR_TYPE_MISSMATCH, 
            "A reference to a '%s' cannot be interpreted as reference to '%s'", 
            zenit_type_to_string(expression_symbol->typeinfo->type), 
            zenit_type_to_string(ref_type->element->type)
        );
    }

    return ref_symbol;
}

/*
 * Function: visit_array_node
 *  To make sure the array initialization is valid, we need to visit every
 *  element to check it against the array's type.
 *
 * Parameters:
 *  ctx - Context object
 *  node - Node to visit
 *
 * Returns:
 *  struct ZenitSymbol* - The arrays's type information
 */
static struct ZenitSymbol* visit_array_node(struct ZenitContext *ctx, struct ZenitArrayNode *array)
{
    struct ZenitSymbol *array_symbol = zenit_utils_get_tmp_symbol(ctx->program, (struct ZenitNode*) array);
    struct ZenitArrayType *array_type = (struct ZenitArrayType*) array_symbol->typeinfo->type;

    // The array type is inferred in the inference pass, so we have information
    // about it, but it can be a struct type that doesn't exist in the symbol
    // table, so the <is_type_defined> function will tell us that
    bool is_array_type_defined = is_type_defined(ctx->program, array_symbol->typeinfo);

    for (size_t i=0; i < fl_array_length(array->elements); i++)
    {
        struct ZenitSymbol* element_symbol = visit_node(ctx, array->elements[i]);

        // If the type check fails, we add an error only if the array type is defined
        // because if not, we might be targeting a false positive error
        if (is_array_type_defined && !zenit_type_is_assignable_from(array_type->member_type->type, element_symbol->typeinfo->type))
        {
            zenit_context_error(ctx, array->elements[i]->location, ZENIT_ERROR_TYPE_MISSMATCH, 
                "Cannot convert from type '%s' to '%s'", 
                zenit_type_to_string(element_symbol->typeinfo->type), 
                zenit_type_to_string(array_type->member_type->type)
            );
        }
    }

    // We return the type of the array initializer
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
 *  By now, this function is pretty dumb because there is nothing to check in the
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
            visit_node(ctx, prop->value);

            if (!is_type_defined(ctx->program, prop_symbol->typeinfo))
            {
                struct ZenitTypeInfo *typeinfo = get_undefined_type(ctx->program, prop_symbol->typeinfo);
        
                // Last resort
                if (typeinfo == NULL)
                    typeinfo = prop_symbol->typeinfo;

                zenit_context_error(ctx, prop->base.location, ZENIT_ERROR_MISSING_SYMBOL, 
                    "In property '%s', type '%s' is not defined", prop->name, zenit_type_to_string(typeinfo->type));
            }
        }
        fl_array_free(properties);
    }
    fl_array_free(names);
}

static struct ZenitSymbol* visit_struct_node(struct ZenitContext *ctx, struct ZenitStructNode *struct_node)
{
    struct ZenitScope *struct_scope = NULL;
    
    if (struct_node->name != NULL)
        struct_scope = zenit_program_get_scope(ctx->program, ZENIT_SCOPE_STRUCT, struct_node->name);

    for (size_t i=0; i < fl_array_length(struct_node->members); i++)
    {
        if (struct_node->members[i]->type == ZENIT_NODE_FIELD)
        {
            struct ZenitFieldNode *field_node = (struct ZenitFieldNode*) struct_node->members[i];
            struct ZenitSymbol *value_symbol = visit_node(ctx, field_node->value);

            if (struct_scope == NULL)
                continue;

            struct ZenitSymbol *field_decl_symbol = zenit_scope_get_symbol(struct_scope, field_node->name);

            if (field_decl_symbol == NULL)
                continue; // The resolve pass should have added an error in this case

            bool is_var_type_defined = is_type_defined(ctx->program, field_decl_symbol->typeinfo);

            // We check types to make sure the assignment is valid, but we do it only if
            // the variable type is valid, because if not, we might be targeting a false-positive
            // error
            if (is_var_type_defined && !zenit_type_is_assignable_from(field_decl_symbol->typeinfo->type, value_symbol->typeinfo->type))
            {
                zenit_context_error(ctx, field_node->base.location, ZENIT_ERROR_TYPE_MISSMATCH, 
                    "Cannot assign from type '%s' to '%s'", 
                    zenit_type_to_string(value_symbol->typeinfo->type), 
                    zenit_type_to_string(field_decl_symbol->typeinfo->type)
                );
            }
        }
    }

    return zenit_utils_get_tmp_symbol(ctx->program, (struct ZenitNode*) struct_node);
}

static struct ZenitSymbol* visit_field_decl_node(struct ZenitContext *ctx, struct ZenitFieldDeclNode *field_node)
{
    return zenit_program_get_symbol(ctx->program, field_node->name);
}

static struct ZenitSymbol* visit_struct_decl_node(struct ZenitContext *ctx, struct ZenitStructDeclNode *struct_node)
{
    // Visit the attributes and its properties
    visit_attribute_node_map(ctx, &struct_node->attributes);

    zenit_program_push_scope(ctx->program, ZENIT_SCOPE_STRUCT, struct_node->name);

    for (size_t i=0; i < fl_array_length(struct_node->members); i++)
        visit_node(ctx, struct_node->members[i]);

    zenit_program_pop_scope(ctx->program);

    return NULL;
}

/*
 * Function: visit_variable_node
 *  The variable visitor makes sure the variable type exists and also makes
 *  sure the right-hand side expression is compatible with it
 *
 * Parameters:
 *  ctx - Context object
 *  node - Node to visit
 *
 * Returns:
 *  struct ZenitSymbol* - The variable's type information
 */
static struct ZenitSymbol* visit_variable_node(struct ZenitContext *ctx, struct ZenitVariableNode *variable_node)
{
    // Visit the attributes
    visit_attribute_node_map(ctx, &variable_node->attributes);

    // We get the variable's symbol
    struct ZenitSymbol *symbol = zenit_program_get_symbol(ctx->program, variable_node->name);

    // Check if the variable's type is native or it is registered in the symbol table
    bool is_var_type_defined = is_type_defined(ctx->program, symbol->typeinfo);

    // If the variable type is missing, we add an error
    if (!is_var_type_defined && symbol->typeinfo->source == ZENIT_TYPE_SRC_HINT)
    {
        struct ZenitTypeInfo *typeinfo = get_undefined_type(ctx->program, symbol->typeinfo);
        
        // Last resort
        if (typeinfo == NULL)
            typeinfo = symbol->typeinfo;

        zenit_context_error(ctx, variable_node->base.location, ZENIT_ERROR_MISSING_SYMBOL, 
            "Type '%s' is not defined", zenit_type_to_string(typeinfo->type));
    }

    // We visit the right-hand side expression to do type checking in it
    struct ZenitSymbol* rhs_symbol = visit_node(ctx, variable_node->rvalue);
    
    // We check types to make sure the assignment is valid, but we do it only if
    // the variable type is valid, because if not, we might be targeting a false-positive
    // error
    if (is_var_type_defined && !zenit_type_is_assignable_from(symbol->typeinfo->type, rhs_symbol->typeinfo->type))
    {
        zenit_context_error(ctx, variable_node->base.location, ZENIT_ERROR_TYPE_MISSMATCH, 
            "Cannot assign from type '%s' to '%s'", 
            zenit_type_to_string(rhs_symbol->typeinfo->type), 
            zenit_type_to_string(symbol->typeinfo->type)
        );
    }

    // The type information returned is always the one from the variable's symbol
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
 *  struct ZenitSymbol* - A valid type information object or NULL
 */
static struct ZenitSymbol* visit_node(struct ZenitContext *ctx, struct ZenitNode *node)
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
