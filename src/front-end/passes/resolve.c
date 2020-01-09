#include "resolve.h"
#include "utils.h"
#include "../program.h"
#include "../symbol.h"
#include "../types/system.h"

/*
 * Type: ZenitSymbolResolver
 *  A resolver function takes the <struct ZenitContext> and a <struct ZenitNode> and
 *  populates the symbol table with symbol information.
 *  The function returns a <struct ZenitSymbol> representing the visited node's result
 */
typedef struct ZenitSymbol*(*ZenitSymbolResolver)(struct ZenitContext *ctx, struct ZenitNode *node);

// Visitor functions
static struct ZenitSymbol* visit_node(struct ZenitContext *ctx, struct ZenitNode *node);
static struct ZenitSymbol* visit_uint_node(struct ZenitContext *ctx, struct ZenitUintNode *node);
static struct ZenitSymbol* visit_variable_node(struct ZenitContext *ctx, struct ZenitVariableNode *node);
static struct ZenitSymbol* visit_array_node(struct ZenitContext *ctx, struct ZenitArrayNode *node);
static struct ZenitSymbol* visit_identifier_node(struct ZenitContext *ctx, struct ZenitIdentifierNode *node);
static struct ZenitSymbol* visit_reference_node(struct ZenitContext *ctx, struct ZenitReferenceNode *node);
static struct ZenitSymbol* visit_cast_node(struct ZenitContext *ctx, struct ZenitCastNode *node);

/*
 * Variable: symbol_resolvers
 *  An array indexed with a <enum ZenitNodeType> to get a <ZenitSymbolResolver> function
 */
static const ZenitSymbolResolver symbol_resolvers[] = {
    [ZENIT_NODE_VARIABLE]   = (ZenitSymbolResolver) &visit_variable_node,
    [ZENIT_NODE_IDENTIFIER] = (ZenitSymbolResolver) &visit_identifier_node,
    [ZENIT_NODE_ARRAY]      = (ZenitSymbolResolver) &visit_array_node,
    [ZENIT_NODE_REFERENCE]  = (ZenitSymbolResolver) &visit_reference_node,
    [ZENIT_NODE_CAST]       = (ZenitSymbolResolver) &visit_cast_node,
    [ZENIT_NODE_UINT]       = (ZenitSymbolResolver) &visit_uint_node,
};

/*
 * Function: visit_uint_node
 *  We create a temporal symbol with the uint type information in it.
 *
 * Parameters:
 *  <struct ZenitContext> *ctx - Context object
 *  <struct ZenitNode> *node - Node object
 *
 * Returns:
 *  struct ZenitSymbol* - Temporal symbol
 *
 */
static struct ZenitSymbol* visit_uint_node(struct ZenitContext *ctx, struct ZenitUintNode *node)
{
    // A uint has an intrinsic type, which means it can't be changed by the inference pass
    struct ZenitUintTypeInfo *uint_typeinfo = zenit_type_uint_new(ZENIT_TYPE_SRC_INTRINSIC, node->size);
    uint_typeinfo->base.sealed = true;
    return zenit_utils_new_tmp_symbol(ctx->program, (struct ZenitNode*) node, (struct ZenitTypeInfo*) uint_typeinfo);
}

/*
 * Function: visit_cast_node
 *  If the cast has a type hint, we need to get that information know to build a temporal symbol that will be
 *  helpful in the type inference and type checking passes
 *
 * Parameters:
 *  <struct ZenitContext> *ctx - Context object
 *  <struct ZenitCastNode> *cast_node - Visited node
 *
 * Returns:
 *  struct ZenitSymbol* - Temporal symbol
 *
 */
static struct ZenitSymbol* visit_cast_node(struct ZenitContext *ctx, struct ZenitCastNode *cast_node)
{
    visit_node(ctx, cast_node->expression);

    // If the type hint is NULL, the <build_type_info_from_declaration> function returns a NONE type, so we are ok.
    // We DON'T make assumptions of the type with the expression being casted, because it is not helpful as the cast's goal is to 
    // "forget" about the original expression's type. If the cast does not have a type hint, we will need information from the context
    // to infer the type.
    return zenit_utils_new_tmp_symbol(ctx->program, (struct ZenitNode*) cast_node, build_type_info_from_declaration(cast_node->type_decl));
}

/*
 * Function: visit_identifier_node
 *  An identifier must be registered in the symbol table to be valid, so this function
 *  performs that check, if the symbol does not exist it registers an error
 *
 * Parameters:
 *  <struct ZenitContext> *ctx - Context object
 *  <struct ZenitIdentifierNode> *id_node - Visited node
 *
 * Returns:
 *  struct ZenitSymbol* - A valid symbol if the identifier is valid, otherwise NULL
 *
 * Notes:
 *  FIXME: In the future we will need a "unresolved symbol" reference to avoid forward declarations
 */
static struct ZenitSymbol* visit_identifier_node(struct ZenitContext *ctx, struct ZenitIdentifierNode *id_node)
{
    // If the symbol exists we return it to the caller
    if (zenit_program_has_symbol(ctx->program, id_node->name))
        return zenit_program_get_symbol(ctx->program, id_node->name);

    // By now, if the identifier does not exist, we add an error (we are not declaring variables or types at the moment, so a missing identifier
    // is a not defined variable)
    zenit_context_error(ctx, id_node->base.location, ZENIT_ERROR_MISSING_SYMBOL, "Identifier %s is not defined.", id_node->name);

    return NULL;
}

/*
 * Function: visit_reference_node
 *  This function adds a new temporal symbol with an "inferred" type information about the reference
 *
 * Parameters:
 *  <struct ZenitContext> *ctx - Context object
 *  <struct ZenitReferenceNode> *reference_node - Visited node
 *
 * Returns:
 *  struct ZenitSymbol* - Temporal symbol
 *
 */
static struct ZenitSymbol* visit_reference_node(struct ZenitContext *ctx, struct ZenitReferenceNode *reference_node)
{
    struct ZenitSymbol *expr_symbol = visit_node(ctx, reference_node->expression);

    if (expr_symbol == NULL)
        return NULL;

    // The source of the reference type comes from the source of the referenced expression
    struct ZenitTypeInfo *typeinfo = (struct ZenitTypeInfo*) zenit_type_reference_new(expr_symbol->typeinfo->source, zenit_type_copy(expr_symbol->typeinfo));

    return zenit_utils_new_tmp_symbol(ctx->program, (struct ZenitNode*) reference_node, typeinfo);
}

/*
 * Function: visit_array_node
 *  In this function we construct a big part of the type information of an array initializer
 *
 * Parameters:
 *  <struct ZenitContext> *ctx - Context object
 *  <struct ZenitArrayNode> *array_node - Visited node
 *
 * Returns:
 *  struct ZenitSymbol* - Temporal symbol
 *
 */
static struct ZenitSymbol* visit_array_node(struct ZenitContext *ctx, struct ZenitArrayNode *array_node)
{
    // We start creating an array type, we flag it as INFERRED because the truth is, at this point we can't make
    // sure of its type, so we will need help/confirmation from the inference and type check passes.
    struct ZenitArrayTypeInfo *array_type = zenit_type_array_new(ZENIT_TYPE_SRC_INFERRED, zenit_type_none_new());
    
    // The length is the number of elements within the array initializer, that's something we know
    array_type->length = fl_array_length(array_node->elements);

    // If there are elements within the array, we can check if all them are equals,
    // in which case we can make sure the array's member_type property can be updated from NONE
    // to the specifc type. In case they are not equals (even though they could be "unified"), we
    // will need to wait for the inference pass to know the exact type of the array
    bool member_type_is_known = array_type->length > 0;
    struct ZenitTypeInfo *first = NULL;
    for (size_t i=0; i < fl_array_length(array_node->elements); i++)
    {
        // On every array entry, we visit the node to get the symbol, and we use that
        // to track the type information of each element
        struct ZenitSymbol *element_symbol = visit_node(ctx, array_node->elements[i]);

        struct ZenitTypeInfo *element_typeinfo = NULL;
        if (element_symbol != NULL)
            element_typeinfo = element_symbol->typeinfo;

        if (i > 0 && member_type_is_known && (element_typeinfo == NULL || !zenit_type_equals(first, element_typeinfo)))
            member_type_is_known = false;
        
        if (first == NULL && element_typeinfo != NULL)
            first = element_typeinfo;
    }

    // If the member type is known, we can free the memory of the NONE type assigned to member_type
    // and copy the new member_type from one of the array elements
    if (member_type_is_known)
    {
        zenit_type_free(array_type->member_type);
        array_type->member_type = zenit_type_copy(first);
    }

    return zenit_utils_new_tmp_symbol(ctx->program, (struct ZenitNode*) array_node, (struct ZenitTypeInfo*) array_type);
}

/*
 * Function: visit_attribute_node_map
 *  We iterate over all the attributes to make sure the properties' values are valid symbols
 *
 * Parameters:
 *  <struct ZenitContext> *ctx - Context object
 *  <struct ZenitAttributeNodeMap> *attributes: Attributes map
 * 
 * Returns:
 *  void - This function does not return a value
 *
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

            // Visit the property's value
            struct ZenitSymbol *value_symbol = visit_node(ctx, prop->value);

            // Similar to the variable declaration, we take the property's type from 
            // its assignment (by now, properties do not have type hints, that's because
            // we are using "textual" attributes)
            struct ZenitTypeInfo *typeinfo = NULL;
            if (value_symbol != NULL)
            {
                typeinfo = zenit_type_copy(value_symbol->typeinfo);
            }
            else
            {
                typeinfo = zenit_type_none_new();
            }

            // Add a temporal symbol for the property
            zenit_utils_new_tmp_symbol(ctx->program, (struct ZenitNode*) prop, typeinfo);
        }
        fl_array_free(properties);
    }
    fl_array_free(names);
}

/*
 * Function: visit_variable_node
 *  In this function we need to insert a new <struct ZenitSymbol> object in the symbol table. In case the
 *  symbol already exists, we need to add an error to the <struct ZenitContext> object
 *
 * Parameters:
 *  <struct ZenitContext> *ctx - Context object
 *  <struct ZenitVariableNode> *variable_node - Visited node
 *
 * Returns:
 *  struct ZenitSymbol* - The new symbol introduced by the variable declaration or NULL on error
 *
 */
static struct ZenitSymbol* visit_variable_node(struct ZenitContext *ctx, struct ZenitVariableNode *variable_node)
{
    // We don't care about the returned symbol -if any- at this pass
    struct ZenitSymbol *rhs_symbol = visit_node(ctx, variable_node->rvalue);

    // If the symbol already exists add an error
    if (zenit_program_has_symbol(ctx->program, variable_node->name))
    {
        zenit_context_error(ctx, variable_node->base.location, ZENIT_ERROR_DUPLICATED_SYMBOL, "Cannot redefine symbol '%s'", variable_node->name);
        return NULL;
    }    

    // Visit the attributes and its properties
    visit_attribute_node_map(ctx, &variable_node->attributes);

    // We need a type for our symbol
    struct ZenitTypeInfo *typeinfo = NULL;
    if (variable_node->type_decl)
    {
        // If the variable has a type hint, we should honor it
        typeinfo = build_type_info_from_declaration(variable_node->type_decl);
    }
    else if (rhs_symbol)
    {
        // If the variable does not contain a type hint, we take the type from the 
        // right-hand side
        typeinfo = zenit_type_copy(rhs_symbol->typeinfo);
        typeinfo->sealed = rhs_symbol->typeinfo->sealed;
    }
    else
    {
        // This means the variable does not contain a type hint and the rhs does not
        // exist, which means there are errors, but we need to continue somehow
        typeinfo = zenit_type_none_new();
    }

    // Create and insert the symbol in the table
    return zenit_program_add_symbol(ctx->program, zenit_symbol_new(variable_node->name, typeinfo));
}

/*
 * Function: visit_node
 *  This function selects the visitor function based on the node's type
 *  and calls the function.
 *
 * Parameters:
 *  ctx - Context object
 *  <struct ZenitNode> *node - Node to visit
 *
 * Returns:
 *  struct ZenitSymbol* - A valid symbol if the visited node (or nodes) defines one or NULL
 *
 */
static struct ZenitSymbol* visit_node(struct ZenitContext *ctx, struct ZenitNode *node)
{
    return symbol_resolvers[node->type](ctx, node);
}


/*
 * Function: zenit_resolve_symbols
 *  We just iterate over the declarations visiting each node
 */
bool zenit_resolve_symbols(struct ZenitContext *ctx)
{
    if (!ctx || !ctx->ast || !ctx->ast->decls)
        return false;

    size_t errors = zenit_context_error_count(ctx);

    for (size_t i=0; i < fl_array_length(ctx->ast->decls); i++)
        visit_node(ctx, ctx->ast->decls[i]);

    return errors == zenit_context_error_count(ctx);
}
