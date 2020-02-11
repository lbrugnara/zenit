#include "check.h"
#include "utils.h"
#include "../program.h"
#include "../symbol.h"

typedef struct ZenitSymbol*(*ZenitTypeChecker)(struct ZenitContext *ctx, struct ZenitNode *node);

// Visitor functions
static struct ZenitSymbol* visit_node(struct ZenitContext *ctx, struct ZenitNode *node);
static struct ZenitSymbol* visit_uint_node(struct ZenitContext *ctx, struct ZenitUintNode *uint_node);
static struct ZenitSymbol* visit_bool_node(struct ZenitContext *ctx, struct ZenitBoolNode *bool_node);
static struct ZenitSymbol* visit_variable_node(struct ZenitContext *ctx, struct ZenitVariableNode *variable_node);
static struct ZenitSymbol* visit_array_node(struct ZenitContext *ctx, struct ZenitArrayNode *array_node);
static struct ZenitSymbol* visit_identifier_node(struct ZenitContext *ctx, struct ZenitIdentifierNode *id_node);
static struct ZenitSymbol* visit_reference_node(struct ZenitContext *ctx, struct ZenitReferenceNode *ref_node);
static struct ZenitSymbol* visit_cast_node(struct ZenitContext *ctx, struct ZenitCastNode *cast_node);
static struct ZenitSymbol* visit_field_decl_node(struct ZenitContext *ctx, struct ZenitFieldDeclNode *field_node);
static struct ZenitSymbol* visit_struct_decl_node(struct ZenitContext *ctx, struct ZenitStructDeclNode *struct_node);
static struct ZenitSymbol* visit_struct_node(struct ZenitContext *ctx, struct ZenitStructNode *struct_node);

/*
 * Variable: checkers
 *  An array indexed with a <enum ZenitNodeKind> to get a <ZenitTypeChecker> function
 */
static const ZenitTypeChecker checkers[] = {
    [ZENIT_NODE_UINT]           = (ZenitTypeChecker) &visit_uint_node,
    [ZENIT_NODE_BOOL]           = (ZenitTypeChecker) &visit_bool_node,
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
 *  <struct ZenitContext> *ctx - Context object
 *  <struct ZenitCastNode> *cast_node - Node to visit
 *
 * Returns:
 *  <struct ZenitSymbol>* - The cast temporal symbol
 */
static struct ZenitSymbol* visit_cast_node(struct ZenitContext *ctx, struct ZenitCastNode *cast_node)
{
    struct ZenitSymbol *symbol = zenit_utils_get_tmp_symbol(ctx->program, (struct ZenitNode*) cast_node);

    struct ZenitSymbol *expr_symbol = visit_node(ctx, cast_node->expression);

    // If the cast is implicit, we need to make sure we can up-cast the expression to the requested type
    if ((cast_node->implicit && !zenit_type_is_assignable_from(symbol->type, expr_symbol->type))
        // HACK: If the cast is explicit, we can check if "the other way around" works for these types, 
        // in that, case we know it is safe to "truncate" the type :grinning:
        || (!cast_node->implicit && !zenit_type_is_castable_to(expr_symbol->type, symbol->type)))
    {
        zenit_context_error(ctx, cast_node->base.location, ZENIT_ERROR_TYPE_MISSMATCH, "Cannot %s from type '%s' to '%s'", 
                cast_node->implicit ? "implicitly cast" : "cast", 
                zenit_type_to_string(expr_symbol->type),
                zenit_type_to_string(symbol->type)
        );
    }

    return symbol;
}

/*
 * Function: visit_uint_node
 *  The uint visitor doesn't need to check anything, it just returns the uint temporal symbol
 *
 * Parameters:
 *  <struct ZenitContext> *ctx - Context object
 *  <struct ZenitUintNode> *uint_node - Node to visit
 *
 * Returns:
 *  <struct ZenitSymbol>* - The uint symbol
 */
static struct ZenitSymbol* visit_uint_node(struct ZenitContext *ctx, struct ZenitUintNode *uint_node)
{
    return zenit_utils_get_tmp_symbol(ctx->program, (struct ZenitNode*) uint_node);
}

/*
 * Function: visit_bool_node
 *  The bool visitor doesn't need to check anything, it just returns the bool temporal symbol
 *
 * Parameters:
 *  <struct ZenitContext> *ctx - Context object
 *  <struct ZenitBoolNode> *bool_node - Node to visit
 *
 * Returns:
 *  <struct ZenitSymbol>* - The bool symbol
 */
static struct ZenitSymbol* visit_bool_node(struct ZenitContext *ctx, struct ZenitBoolNode *bool_node)
{
    return zenit_utils_get_tmp_symbol(ctx->program, (struct ZenitNode*) bool_node);
}

/*
 * Function: visit_identifier_node
 *  It just returns the identifier symbol
 *
 * Parameters:
 *  <struct ZenitContext> *ctx - Context object
 *  <struct ZenitIdentifierNode> *id_node - Node to visit
 *
 * Returns:
 *  <struct ZenitSymbol>* - The identifier type information
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
 *  <struct ZenitContext> *ctx - Context object
 *  <struct ZenitReferenceNode> *reference_node - Node to visit
 *
 * Returns:
 *  <struct ZenitSymbol>* - The reference symbol
 */
static struct ZenitSymbol* visit_reference_node(struct ZenitContext *ctx, struct ZenitReferenceNode *reference_node)
{
    struct ZenitSymbol *ref_symbol = zenit_utils_get_tmp_symbol(ctx->program, (struct ZenitNode*) reference_node);
    struct ZenitReferenceType *ref_type = (struct ZenitReferenceType*) ref_symbol->type;

    struct ZenitSymbol *expression_symbol = visit_node(ctx, reference_node->expression);

    // FIXME: Cannot take a reference to a temporal symbol (temporal expression, primitive, etc)
    if (expression_symbol->name[0] == '%' && expression_symbol->type->typekind == ZENIT_TYPE_REFERENCE)
    {
        zenit_context_error(ctx, reference_node->base.location, ZENIT_ERROR_INVALID_REFERENCE, 
                "Cannot take a reference to another reference.");
    }

    if (!zenit_type_is_assignable_from(ref_type->element, expression_symbol->type))
    {
        zenit_context_error(ctx, reference_node->base.location, ZENIT_ERROR_TYPE_MISSMATCH, 
            "A reference to a '%s' cannot be interpreted as reference to '%s'", 
            zenit_type_to_string(expression_symbol->type), 
            zenit_type_to_string(ref_type->element)
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
 *  <struct ZenitContext> *ctx - Context object
 *  <struct ZenitArrayNode> *array_node - Node to visit
 *
 * Returns:
 *  <struct ZenitSymbol>* - The arrays symbol
 */
static struct ZenitSymbol* visit_array_node(struct ZenitContext *ctx, struct ZenitArrayNode *array_node)
{
    struct ZenitSymbol *array_symbol = zenit_utils_get_tmp_symbol(ctx->program, (struct ZenitNode*) array_node);
    struct ZenitArrayType *array_type = (struct ZenitArrayType*) array_symbol->type;

    bool is_valid_type = zenit_program_is_valid_type(ctx->program, array_symbol->type);

    for (size_t i=0; i < fl_array_length(array_node->elements); i++)
    {
        struct ZenitSymbol* element_symbol = visit_node(ctx, array_node->elements[i]);

        // If the type check fails, we add an error only if the array type is defined
        // because if not, we might be targeting a false positive error
        if (is_valid_type && !zenit_type_is_assignable_from(array_type->member_type, element_symbol->type))
        {
            zenit_context_error(ctx, array_node->elements[i]->location, ZENIT_ERROR_TYPE_MISSMATCH, 
                "Cannot convert from type '%s' to '%s'", 
                zenit_type_to_string(element_symbol->type), 
                zenit_type_to_string(array_type->member_type)
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
 *  <ZenitAttributeNodeMap> *attributes: Attributes map
 * 
 * Returns:
 *  void - This function does not return a value
 *
 * Notes:
 *  By now, this function is pretty dumb because there is nothing to check in the
 *  properties
 */
static void visit_attribute_node_map(struct ZenitContext *ctx, ZenitAttributeNodeMap *attributes)
{
    const char **names = zenit_attribute_node_map_keys(attributes);
    for (size_t i=0; i < fl_array_length(names); i++)
    {
        struct ZenitAttributeNode *attr = zenit_attribute_node_map_get(attributes, names[i]);

        struct ZenitPropertyNode **properties = zenit_property_node_map_values(attr->properties);
        for (size_t j=0; j < fl_array_length(properties); j++)
        {
            struct ZenitPropertyNode *prop = properties[j];
            struct ZenitSymbol *prop_symbol = zenit_utils_get_tmp_symbol(ctx->program, (struct ZenitNode*) prop);
            visit_node(ctx, prop->value);

            if (!zenit_program_is_valid_type(ctx->program, prop_symbol->type))
            {
                struct ZenitType *type = zenit_program_get_invalid_type_component(ctx->program, prop_symbol->type);
        
                // Last resort
                if (type == NULL)
                    type = prop_symbol->type;

                zenit_context_error(ctx, prop->base.location, ZENIT_ERROR_MISSING_SYMBOL, 
                    "In property '%s', type '%s' is not defined", prop->name, zenit_type_to_string(type));
            }
        }
        fl_array_free(properties);
    }
    fl_array_free(names);
}

/*
 * Function: visit_struct_node
 *  The struct literal visitor makes type check of the struct fields, but only for named structs. Unnamed structs
 *  do not have a declared type in its field, and because of that they are 100% inferred, which means that type
 *  checking the struct type member agains the value member will always returns true. On the other hand, named structs
 *  have a declared type in their fields, and because of that we need to check those types with the types of the
 *  fields initializers
 *
 * Parameters:
 *  <struct ZenitContext> *ctx - Context object
 *  <struct ZenitStructNode> *struct_node - Node to visit
 *
 * Returns:
 *  <struct ZenitSymbol>* - The struct symbol with its type information
 */
static struct ZenitSymbol* visit_struct_node(struct ZenitContext *ctx, struct ZenitStructNode *struct_node)
{
    struct ZenitScope *struct_scope = NULL;
    
    if (struct_node->name != NULL)
        struct_scope = zenit_program_get_scope(ctx->program, ZENIT_SCOPE_STRUCT, struct_node->name);

    for (size_t i=0; i < fl_array_length(struct_node->members); i++)
    {
        if (struct_node->members[i]->nodekind == ZENIT_NODE_FIELD)
        {
            struct ZenitFieldNode *field_node = (struct ZenitFieldNode*) struct_node->members[i];
            struct ZenitSymbol *value_symbol = visit_node(ctx, field_node->value);

            // NOTE: For unnamed structs, we don't have an scope, and the type of the member should be determined
            // in the inference pass, so it is not necessary to type check it, as it should be equals
            // to the value's type
            if (struct_scope == NULL)
                continue;

            // NOTE: For named structs we need to type check against the field declaration
            struct ZenitSymbol *field_decl_symbol = zenit_scope_get_symbol(struct_scope, field_node->name);

            if (field_decl_symbol == NULL)
                continue; // The resolve pass should have added an error in this case

            bool is_valid_type = zenit_program_is_valid_type(ctx->program, field_decl_symbol->type);

            // We check types to make sure the assignment is valid, but we do it only if
            // the field type is valid, because if not, we might be targeting a false-positive
            // error
            if (is_valid_type && !zenit_type_is_assignable_from(field_decl_symbol->type, value_symbol->type))
            {
                zenit_context_error(ctx, field_node->base.location, ZENIT_ERROR_TYPE_MISSMATCH, 
                    "Cannot assign from type '%s' to a member of type '%s'", 
                    zenit_type_to_string(value_symbol->type), 
                    zenit_type_to_string(field_decl_symbol->type)
                );
            }
        }
    }

    return zenit_utils_get_tmp_symbol(ctx->program, (struct ZenitNode*) struct_node);
}

/*
 * Function: visit_field_decl_node
 *  The field declaration visitor returns the field symbol.
 *
 * Parameters:
 *  <struct ZenitContext> *ctx - Context object
 *  <struct ZenitFieldDeclNode> *field_node - Node to visit
 *
 * Returns:
 *  <struct ZenitSymbol>* - The field symbol with its type information
 */
static struct ZenitSymbol* visit_field_decl_node(struct ZenitContext *ctx, struct ZenitFieldDeclNode *field_node)
{
    return zenit_program_get_symbol(ctx->program, field_node->name);
}

/*
 * Function: visit_struct_decl_node
 *  The struct declaration visitor pushes its scope as the current scope in the program and iterates over its members
 *
 * Parameters:
 *  <struct ZenitContext> *ctx - Context object
 *  <struct ZenitStructDeclNode> *struct_node - Node to visit
 *
 * Returns:
 *  <struct ZenitSymbol>* - This function returns NULL as there is not symbol associated with the struct declaration
 */
static struct ZenitSymbol* visit_struct_decl_node(struct ZenitContext *ctx, struct ZenitStructDeclNode *struct_node)
{
    // Visit the attributes and its properties
    visit_attribute_node_map(ctx, struct_node->attributes);

    zenit_program_push_scope(ctx->program, ZENIT_SCOPE_STRUCT, struct_node->name);

    for (size_t i=0; i < fl_array_length(struct_node->members); i++)
        visit_node(ctx, struct_node->members[i]);

    zenit_program_pop_scope(ctx->program);

    return NULL;
}

/*
 * Function: visit_variable_node
 *  The variable visitor makes sure the variable type exists and also makes
 *  sure the right-hand side expression is compatible with the left-hand side.
 *
 * Parameters:
 *  <struct ZenitContext> *ctx - Context object
 *  <struct ZenitVariableNode> *variable_node - Node to visit
 *
 * Returns:
 *  <struct ZenitSymbol>* - The variable symbol
 */
static struct ZenitSymbol* visit_variable_node(struct ZenitContext *ctx, struct ZenitVariableNode *variable_node)
{
    // Visit the attributes
    visit_attribute_node_map(ctx, variable_node->attributes);

    // We get the variable's symbol
    struct ZenitSymbol *symbol = zenit_program_get_symbol(ctx->program, variable_node->name);

    // Check if the variable's type is native or it is registered in the symbol table
    bool is_valid_type = zenit_program_is_valid_type(ctx->program, symbol->type);

    // If the variable type is missing, we add an error
    if (!is_valid_type && variable_node->type_decl != NULL)
    {
        struct ZenitType *type = zenit_program_get_invalid_type_component(ctx->program, symbol->type);
        
        // Last resort
        if (type == NULL)
            type = symbol->type;

        zenit_context_error(ctx, variable_node->base.location, ZENIT_ERROR_MISSING_SYMBOL, 
            "Type '%s' is not defined", zenit_type_to_string(type));
    }

    // We visit the right-hand side expression to do type checking with it
    struct ZenitSymbol* rhs_symbol = visit_node(ctx, variable_node->rvalue);
    
    // We check types to make sure the assignment is valid, but we do it only if
    // the variable type is valid, because if not, we might be targeting a false-positive
    // error
    if (is_valid_type && !zenit_type_is_assignable_from(symbol->type, rhs_symbol->type))
    {
        zenit_context_error(ctx, variable_node->base.location, ZENIT_ERROR_TYPE_MISSMATCH, 
            "Cannot assign from type '%s' to '%s'", 
            zenit_type_to_string(rhs_symbol->type), 
            zenit_type_to_string(symbol->type)
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
 *  <struct ZenitContext> *ctx - Context object
 *  <struct ZenitNode> *node - Node to visit
 *
 * Returns:
 *  <struct ZenitSymbol>* - The symbol tied to the AST node
 */
static struct ZenitSymbol* visit_node(struct ZenitContext *ctx, struct ZenitNode *node)
{
    return checkers[node->nodekind](ctx, node);
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
