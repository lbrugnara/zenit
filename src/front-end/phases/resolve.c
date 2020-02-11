#include "resolve.h"
#include "utils.h"
#include "../program.h"
#include "../symbol.h"
#include "../types/context.h"

enum ResolvePass {
    RESOLVE_STRUCT,
    RESOLVE_ALL
};

/*
 * Type: ZenitSymbolResolver
 *  A resolver function takes the <struct ZenitContext>, a <struct ZenitNode>, and a <enum ResolvePass>
 *  to populates the symbol table with symbol information.
 *  The function returns a <struct ZenitSymbol> representing the visited node's result
 */
typedef struct ZenitSymbol*(*ZenitSymbolResolver)(struct ZenitContext *ctx, struct ZenitNode *node, enum ResolvePass pass);

// Visitor functions
static struct ZenitSymbol* visit_node(struct ZenitContext *ctx, struct ZenitNode *node, enum ResolvePass pass);
static struct ZenitSymbol* visit_uint_node(struct ZenitContext *ctx, struct ZenitUintNode *uint_node, enum ResolvePass pass);
static struct ZenitSymbol* visit_bool_node(struct ZenitContext *ctx, struct ZenitBoolNode *bool_node, enum ResolvePass pass);
static struct ZenitSymbol* visit_variable_node(struct ZenitContext *ctx, struct ZenitVariableNode *Variable_node, enum ResolvePass pass);
static struct ZenitSymbol* visit_array_node(struct ZenitContext *ctx, struct ZenitArrayNode *array_node, enum ResolvePass pass);
static struct ZenitSymbol* visit_identifier_node(struct ZenitContext *ctx, struct ZenitIdentifierNode *id_node, enum ResolvePass pass);
static struct ZenitSymbol* visit_reference_node(struct ZenitContext *ctx, struct ZenitReferenceNode *ref_node, enum ResolvePass pass);
static struct ZenitSymbol* visit_cast_node(struct ZenitContext *ctx, struct ZenitCastNode *cast_node, enum ResolvePass pass);
static struct ZenitSymbol* visit_field_decl_node(struct ZenitContext *ctx, struct ZenitFieldDeclNode *field_node, enum ResolvePass pass);
static struct ZenitSymbol* visit_struct_decl_node(struct ZenitContext *ctx, struct ZenitStructDeclNode *struct_node, enum ResolvePass pass);
static struct ZenitSymbol* visit_struct_node(struct ZenitContext *ctx, struct ZenitStructNode *struct_node, enum ResolvePass pass);

/*
 * Variable: symbol_resolvers
 *  An array indexed with a <enum ZenitNodeKind> to get a <ZenitSymbolResolver> function
 */
static const ZenitSymbolResolver symbol_resolvers[] = {
    [ZENIT_NODE_VARIABLE]       = (ZenitSymbolResolver) &visit_variable_node,
    [ZENIT_NODE_IDENTIFIER]     = (ZenitSymbolResolver) &visit_identifier_node,
    [ZENIT_NODE_ARRAY]          = (ZenitSymbolResolver) &visit_array_node,
    [ZENIT_NODE_REFERENCE]      = (ZenitSymbolResolver) &visit_reference_node,
    [ZENIT_NODE_CAST]           = (ZenitSymbolResolver) &visit_cast_node,
    [ZENIT_NODE_UINT]           = (ZenitSymbolResolver) &visit_uint_node,
    [ZENIT_NODE_BOOL]           = (ZenitSymbolResolver) &visit_bool_node,
    [ZENIT_NODE_FIELD_DECL]     = (ZenitSymbolResolver) &visit_field_decl_node,
    [ZENIT_NODE_STRUCT_DECL]    = (ZenitSymbolResolver) &visit_struct_decl_node,
    [ZENIT_NODE_STRUCT]         = (ZenitSymbolResolver) &visit_struct_node,
};

/*
 * Function: visit_uint_node
 *  We create a temporal symbol with the uint type information in it.
 *
 * Parameters:
 *  <struct ZenitContext> *ctx - Context object
 *  <struct ZenitUintNode> *uint_node - Node object
 *
 * Returns:
 *  struct ZenitSymbol* - Temporal symbol
 *
 */
static struct ZenitSymbol* visit_uint_node(struct ZenitContext *ctx, struct ZenitUintNode *uint_node, enum ResolvePass pass)
{
    if (pass != RESOLVE_ALL)
        return NULL;

    // We add a temporal symbol for the uint and we return it
    return zenit_utils_new_tmp_symbol(ctx->program, (struct ZenitNode*) uint_node, (struct ZenitType*) zenit_type_ctx_new_uint(ctx->types, uint_node->size));
}

/*
 * Function: visit_bool_node
 *  We create a temporal symbol with the boolean type information in it.
 *
 * Parameters:
 *  <struct ZenitContext> *ctx - Context object
 *  <struct ZenitBoolNode> *bool_node - Node object
 *
 * Returns:
 *  struct ZenitSymbol* - Temporal symbol
 *
 */
static struct ZenitSymbol* visit_bool_node(struct ZenitContext *ctx, struct ZenitBoolNode *bool_node, enum ResolvePass pass)
{
    if (pass != RESOLVE_ALL)
        return NULL;

    // We add a temporal symbol for the bool and we return it
    return zenit_utils_new_tmp_symbol(ctx->program, (struct ZenitNode*) bool_node, (struct ZenitType*) zenit_type_ctx_new_bool(ctx->types));
}

/*
 * Function: visit_cast_node
 *  If the cast has a type hint, we need to get that information know to build a temporal symbol that will be
 *  helpful in the type inference and type checking phases
 *
 * Parameters:
 *  <struct ZenitContext> *ctx - Context object
 *  <struct ZenitCastNode> *cast_node - Visited node
 *
 * Returns:
 *  struct ZenitSymbol* - Temporal symbol
 *
 */
static struct ZenitSymbol* visit_cast_node(struct ZenitContext *ctx, struct ZenitCastNode *cast_node, enum ResolvePass pass)
{
    if (pass != RESOLVE_ALL)
        return NULL;

    struct ZenitSymbol *expr_symbol = visit_node(ctx, cast_node->expression, pass);

    struct ZenitType *cast_decl = get_type_from_type_declaration(ctx, cast_node->type_decl, expr_symbol != NULL ? expr_symbol->type : NULL);

    // If the type hint is NULL, the <get_type_from_type_declaration> function returns a NONE type, so we are ok.
    // We DON'T make assumptions of the type with the expression being casted, because it is not helpful as the cast's goal is to 
    // "forget" about the original expression's type. If the cast does not have a type hint, we will need information from the context
    // to infer the type.
    return zenit_utils_new_tmp_symbol(ctx->program, (struct ZenitNode*) cast_node, cast_decl);
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
static struct ZenitSymbol* visit_identifier_node(struct ZenitContext *ctx, struct ZenitIdentifierNode *id_node, enum ResolvePass pass)
{
    if (pass != RESOLVE_ALL)
        return NULL;

    // If the symbol exists we return it to the caller
    if (zenit_program_has_symbol(ctx->program, id_node->name))
        return zenit_program_get_symbol(ctx->program, id_node->name);

    // If the identifier does not exist, we add an error
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
static struct ZenitSymbol* visit_reference_node(struct ZenitContext *ctx, struct ZenitReferenceNode *reference_node, enum ResolvePass pass)
{
    if (pass != RESOLVE_ALL)
        return NULL;

    struct ZenitSymbol *expr_symbol = visit_node(ctx, reference_node->expression, pass);

    if (expr_symbol == NULL)
        return NULL;

    return zenit_utils_new_tmp_symbol(ctx->program, (struct ZenitNode*) reference_node, 
        (struct ZenitType*) zenit_type_ctx_new_reference(ctx->types, zenit_type_ctx_copy_type(ctx->types, expr_symbol->type)));
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
static struct ZenitSymbol* visit_array_node(struct ZenitContext *ctx, struct ZenitArrayNode *array_node, enum ResolvePass pass)
{
    if (pass != RESOLVE_ALL)
        return NULL;

    // We start creating an array type, we flag it as INFERRED because the truth is, at this point we can't make
    // sure of its type, so we will need help/confirmation from the inference and type check phases.
    struct ZenitArrayType *array_type = zenit_type_ctx_new_array(ctx->types, zenit_type_ctx_new_none(ctx->types));
    
    // The length is the number of elements within the array initializer, that's something we know
    array_type->length = fl_array_length(array_node->elements);

    // If there are elements within the array, we can check if all them are equals,
    // in which case we can make sure the array's member_type property can be updated from NONE
    // to the specifc type. In case they are not equals (even though they could be "unified"), we
    // will need to wait for the inference pass to know the exact type of the array
    bool member_type_is_known = array_type->length > 0;
    struct ZenitType *last = NULL;
    for (size_t i=0; i < fl_array_length(array_node->elements); i++)
    {
        // On every array entry, we visit the node to get the symbol, and we use that
        // to track the type information of each element
        struct ZenitSymbol *element_symbol = visit_node(ctx, array_node->elements[i], pass);

        // If any of these objects is NULL, the member type cannot be known in the resolve pass
        if (element_symbol == NULL || element_symbol->type == NULL)
            member_type_is_known = false;

        // If we already know it is unkown, just skip the checks
        if (!member_type_is_known)
            continue;

        // Check if the types are still equals, in case they are not, just set the flag to false to
        // skip the checks
        if (last != NULL && !zenit_type_equals(last, element_symbol->type))
            member_type_is_known = false;
        
        // Keep the last visited element's type
        last = element_symbol->type;
    }

    // If the member type is known, we can free the memory of the NONE type assigned to member_type
    // and copy the new member_type from one of the array elements
    if (member_type_is_known)
        array_type->member_type = zenit_type_ctx_copy_type(ctx->types, last);

    return zenit_utils_new_tmp_symbol(ctx->program, (struct ZenitNode*) array_node, (struct ZenitType*) array_type);
}

/*
 * Function: visit_attribute_node_map
 *  We iterate over all the attributes to make sure the properties' values are valid symbols
 *
 * Parameters:
 *  <struct ZenitContext> *ctx - Context object
 *  <ZenitAttributeNodeMap> *attributes: Attributes map
 * 
 * Returns:
 *  void - This function does not return a value
 *
 */
static void visit_attribute_node_map(struct ZenitContext *ctx, ZenitAttributeNodeMap *attributes, enum ResolvePass pass)
{
    if (pass != RESOLVE_ALL)
        return;

    const char **names = zenit_attribute_node_map_keys(attributes);
    for (size_t i=0; i < fl_array_length(names); i++)
    {
        struct ZenitAttributeNode *attr = zenit_attribute_node_map_get(attributes, names[i]);

        struct ZenitPropertyNode **properties = zenit_property_node_map_values(attr->properties);
        for (size_t j=0; j < fl_array_length(properties); j++)
        {
            struct ZenitPropertyNode *prop = properties[j];

            // Visit the property's value
            struct ZenitSymbol *value_symbol = visit_node(ctx, prop->value, pass);

            // Similar to the variable declaration, we take the property's type from 
            // its assignment (by now, properties do not have type hints, that's because
            // we are using "textual" attributes)
            // Add a temporal symbol for the property
            zenit_utils_new_tmp_symbol(ctx->program, (struct ZenitNode*) prop, value_symbol != NULL 
                ? zenit_type_ctx_copy_type(ctx->types, value_symbol->type) 
                : zenit_type_ctx_new_none(ctx->types));
        }
        fl_array_free(properties);
    }
    fl_array_free(names);
}

/*
 * Function: visit_named_struct_node
 *  For named structs we do need a previously registered scope, and we need to retrieve the type created in the RESOLVE_STRUCT pass.
 *  A literal named struct must have values for all its members, so that check can be done here using the struct type.
 * 
 * Parameters:
 *  <struct ZenitContext> *ctx: Context object
 *  <struct ZenitStructNode> *struct_node: Visited node
 *  <enum ResolvePass> pass: The current resolve pass
 *
 * Returns:
 *  struct ZenitSymbol* - The unnamed struct literal symbol
 *
 */
static struct ZenitSymbol* visit_named_struct_node(struct ZenitContext *ctx, struct ZenitStructNode *struct_node, enum ResolvePass pass)
{
    if (pass != RESOLVE_ALL)
        return NULL;

    // We need the struct scope to get the declared fields
    struct ZenitScope *struct_scope = zenit_program_get_scope(ctx->program, ZENIT_SCOPE_STRUCT, struct_node->name);
    if (struct_scope == NULL)
    {
        zenit_context_error(ctx, struct_node->base.location, ZENIT_ERROR_MISSING_SYMBOL, "struct type '%s' is not defined", struct_node->name);
        return NULL;
    }

    // Now we retrieve all the symbols (except for the temporals)
    struct ZenitSymbol **declared_fields = zenit_scope_get_symbols(struct_scope, false);

    // At this point, a named struct type must be registered and be valid
    struct ZenitStructType *struct_type = zenit_type_ctx_get_named_struct(ctx->types, struct_node->name);

    for (size_t i=0; i < fl_array_length(struct_node->members); i++)
    {
        if (struct_node->members[i]->nodekind == ZENIT_NODE_FIELD)
        {
            struct ZenitFieldNode *field_node = (struct ZenitFieldNode*) struct_node->members[i];

            // If the symbol is not present in the scope, it is not a valid field name for this named struct
            if (!zenit_scope_has_symbol(struct_scope, field_node->name))
            {
                zenit_context_error(ctx, field_node->base.location, ZENIT_ERROR_UNKNOWN_MEMBER,
                    "Member '%s' does not exist in struct %s", field_node->name, struct_node->name);
            }
            else
            {
                // We nullify the name of the declared field that matches to this field so we can
                // later check for non-null entries that are the uninitialized members.
                // TODO: We can avoid the nested loop, but for now it is ok
                for (size_t i=0; i < fl_array_length(declared_fields); i++)
                {
                    if (declared_fields[i] == NULL)
                        continue;

                    if (flm_cstring_equals(field_node->name, declared_fields[i]->name))
                    {
                        declared_fields[i] = NULL;
                        break;
                    }
                }
            }

            // Visit the field's value
            visit_node(ctx, field_node->value, pass);
        }
        else
        {
            zenit_context_error(ctx, struct_node->members[i]->location, ZENIT_ERROR_INTERNAL, "Unhandled struct member cannot be resolved.");
        }
    }

    // Now we iterate the declared_fields variable to find uninitialized members, and in case there
    // are missing members in the struct literal, we add an error
    char *uninitialized_members = NULL;
        
    for (size_t i=0; i < fl_array_length(declared_fields); i++)
    {
        if (declared_fields[i] == NULL)
            continue;

        if (uninitialized_members == NULL)
            uninitialized_members = fl_cstring_dup(declared_fields[i]->name);
        else
            fl_cstring_vappend(&uninitialized_members, ", %s", declared_fields[i]->name);
    }

    if (uninitialized_members != NULL)
    {
        zenit_context_error(ctx, struct_node->base.location, ZENIT_ERROR_UNINITIALIZED_MEMBER, 
                "Instance of struct %s has uninitialized members (%s)", struct_node->name, uninitialized_members);

        fl_cstring_free(uninitialized_members);
    }

    fl_array_free(declared_fields);

    // We add the temporal symbol for this literal to the program and we return the symbol to the caller
    return zenit_utils_new_tmp_symbol(ctx->program, (struct ZenitNode*) struct_node, (struct ZenitType*) struct_type);
}

/*
 * Function: visit_unnamed_struct_node
 *  An unnamed struct does not contain a scope nor a declared type. The scope is not needed (???), but we can create the unnamed
 *  struct type here and populate the struct type's members with the information that comes out from the fields visitors. Later in
 *  the inference pass the type will be updated if needed.
 * 
 * Parameters:
 *  <struct ZenitContext> *ctx: Context object
 *  <struct ZenitStructNode> *struct_node: Visited node
 *  <enum ResolvePass> pass: The current resolve pass
 *
 * Returns:
 *  struct ZenitSymbol* - The unnamed struct literal symbol
 *
 */
static struct ZenitSymbol* visit_unnamed_struct_node(struct ZenitContext *ctx, struct ZenitStructNode *struct_node, enum ResolvePass pass)
{
    if (pass != RESOLVE_ALL)
        return NULL;

    // We create the unnamed struct type
    struct ZenitStructType *struct_type = zenit_type_ctx_new_struct(ctx->types, struct_node->name);

    for (size_t i=0; i < fl_array_length(struct_node->members); i++)
    {
        if (struct_node->members[i]->nodekind == ZENIT_NODE_FIELD)
        {
            // We visit the field's value (the initializer) and then we add the struct type member based
            // on the field's name and its value type
            struct ZenitFieldNode *field_node = (struct ZenitFieldNode*) struct_node->members[i];
            struct ZenitSymbol *value_symbol = visit_node(ctx, field_node->value, pass);

            zenit_type_struct_add_member(struct_type, field_node->name, 
                value_symbol != NULL 
                    ? zenit_type_ctx_copy_type(ctx->types, value_symbol->type) 
                    : zenit_type_ctx_new_none(ctx->types));
        }
        else
        {
            zenit_context_error(ctx, struct_node->members[i]->location, ZENIT_ERROR_INTERNAL, "Unhandled struct member cannot be resolved.");
        }
    }

    // We add the temporal symbol for this literal to the program and we return the symbol to the caller
    return zenit_utils_new_tmp_symbol(ctx->program, (struct ZenitNode*) struct_node, (struct ZenitType*) struct_type);
}

/*
 * Function: visit_struct_node
 *  A struct literal can be named or unnamed, so for easy handling we split the visitor in two, this function is just a proxy to
 *  the two different visitors.
 * 
 * Parameters:
 *  <struct ZenitContext> *ctx: Context object
 *  <struct ZenitStructNode> *struct_node: Visited node
 *  <enum ResolvePass> pass: The current resolve pass
 *
 * Returns:
 *  struct ZenitSymbol* - The struct literal symbol
 *
 */
static struct ZenitSymbol* visit_struct_node(struct ZenitContext *ctx, struct ZenitStructNode *struct_node, enum ResolvePass pass)
{
    if (struct_node->name != NULL)
        return visit_named_struct_node(ctx, struct_node, pass);
    
    return visit_unnamed_struct_node(ctx, struct_node, pass);
}

/*
 * Function: visit_field_decl_node
 *  The field declaration visitor run only in the RESOLVE_ALL pass and it registers the field symbol in the current scope (a struct scope by now).
 *  This function checks if the type of the field is valid, because there shouldn't be missing symbols at this point, as the RESOLVE_STRUCT pass should
 *  have registered all the user-defined types.
 *
 * Parameters:
 *  <struct ZenitContext> *ctx: Context object
 *  <struct ZenitFieldDeclNode> *field_node: Visited node
 *  <enum ResolvePass> pass: The current resolve pass
 *
 * Returns:
 *  struct ZenitSymbol* - The new symbol introduced by the field declaration or NULL on error
 *
 */
static struct ZenitSymbol* visit_field_decl_node(struct ZenitContext *ctx, struct ZenitFieldDeclNode *field_node, enum ResolvePass pass)
{
    if (pass != RESOLVE_ALL)
        return NULL;

    // If the symbol already exists add an error
    if (zenit_program_has_symbol(ctx->program, field_node->name))
    {
        zenit_context_error(ctx, field_node->base.location, ZENIT_ERROR_DUPLICATED_SYMBOL, 
            "Field '%s' already exists in struct %s and cannot be redefined", field_node->name, ((struct ZenitStructDeclNode*) field_node->owner)->name);
        return NULL;
    }

    // We need a type for our symbol
    struct ZenitType *type = get_type_from_type_declaration(ctx, field_node->type_decl, NULL);

    if (!zenit_program_is_valid_type(ctx->program, type))
    {
        struct ZenitType *missing_type = zenit_program_get_invalid_type_component(ctx->program, type);
        zenit_context_error(ctx, field_node->base.location, ZENIT_ERROR_MISSING_SYMBOL, "Type '%s' is not defined", zenit_type_to_string(missing_type));
    }

    // Create and insert the symbol in the table
    return zenit_program_add_symbol(ctx->program, zenit_symbol_new(field_node->name, type));
}

/*
 * Function: visit_struct_decl_node
 *  The struct declaration supports all the passes:
 *      - In the first pass it registers the struct within the program to avoid the need of forward declarations
 *      - In the second pass it visits its members to register them in the struct's scope
 *
 * Parameters:
 *  <struct ZenitContext> *ctx: Context object
 *  <struct ZenitStructDeclNode> *struct_node: Visited node
 *  <enum ResolvePass> pass: The current resolve pass
 *
 * Returns:
 *  struct ZenitSymbol* - This function always returns NULL as a struct declaration does not introduce a symbol but a scope
 *
 */
static struct ZenitSymbol* visit_struct_decl_node(struct ZenitContext *ctx, struct ZenitStructDeclNode *struct_node, enum ResolvePass pass)
{
    if (pass == RESOLVE_STRUCT)
    {
        // If the symbol already exists add an error
        if (zenit_program_has_scope(ctx->program, ZENIT_SCOPE_STRUCT, struct_node->name))
        {
            zenit_context_error(ctx, struct_node->base.location, ZENIT_ERROR_DUPLICATED_SYMBOL, "struct %s cannot be redefined", struct_node->name);
            return NULL;
        }

        // Create the struct scope
        zenit_program_add_scope(ctx->program, ZENIT_SCOPE_STRUCT, struct_node->name);

        return NULL;
    }

    // First we need to retrieve the struct scope, but we don't enter yet...
    struct ZenitScope *scope = zenit_program_get_scope(ctx->program, ZENIT_SCOPE_STRUCT, struct_node->name);

    // If there are symbols in the RESOLVE_ALL pass, it means we've already been here, it is an error
    if (zenit_scope_has_symbols(scope))
    {
        //  e.g.:
        //      struct B { b: uint8; }  <-- This RESOLVE_ALL pass populates the scope with the symbol 'b'
        //      struct B { a: uint16; } <-- This RESOLVE_ALL pass checks if the scope contains symbols, and it actually does
        //
        // In this scenario, the RESOLVE_STRUCT pass added an error in the context at the moment it visited the second statement, because
        // struct B was already defined at that point, because of that, the second RESOLVE_ALL pass is not needed at all, we already know
        // it is an error, and it is safe to leave this function
        //
        return NULL;
    }

    // Visit the attributes and its properties (outside of the struct's scope)
    // TODO: The attributes are pretty simple right now, we should check if this visit should
    // happen outside or inside the struct scope
    visit_attribute_node_map(ctx, struct_node->attributes, pass);

    // Now it's time to enter to the scope of the struct to visit its members
    zenit_program_enter_scope(ctx->program, scope);

    // We register a new struct type in the type context so we start populating member information about it
    struct ZenitStructType *struct_type = zenit_type_ctx_new_struct(ctx->types, struct_node->name);

    for (size_t i=0; i < fl_array_length(struct_node->members); i++)
    {
        struct ZenitSymbol *field_symbol = visit_node(ctx, struct_node->members[i], pass);

        if (field_symbol == NULL)
            continue; // An error ocurred on the field decl node

        // We register the field in the struct type
        zenit_type_struct_add_member(struct_type, field_symbol->name, field_symbol->type);
    }

    // We pop the scope and continue
    zenit_program_pop_scope(ctx->program);

    return NULL;
}

/*
 * Function: visit_variable_node
 *  In this function we need to insert a new <struct ZenitSymbol> object in the symbol table. In case the
 *  symbol already exists, we need to add an error to the <struct ZenitContext> object
 *
 * Parameters:
 *  <struct ZenitContext> *ctx: Context object
 *  <struct ZenitVariableNode> *variable_node: Visited node
 *  <enum ResolvePass> pass: The current resolve pass
 *
 * Returns:
 *  struct ZenitSymbol* - The new symbol introduced by the variable declaration or NULL on error
 *
 */
static struct ZenitSymbol* visit_variable_node(struct ZenitContext *ctx, struct ZenitVariableNode *variable_node, enum ResolvePass pass)
{
    if (pass != RESOLVE_ALL)
        return NULL;

    // We don't care about the returned symbol -if any- at this pass
    struct ZenitSymbol *rhs_symbol = visit_node(ctx, variable_node->rvalue, pass);

    // If the symbol already exists add an error
    if (zenit_program_has_symbol(ctx->program, variable_node->name))
    {
        zenit_context_error(ctx, variable_node->base.location, ZENIT_ERROR_DUPLICATED_SYMBOL, "Cannot redefine symbol '%s'", variable_node->name);
        return NULL;
    }

    // Visit the attributes and its properties
    visit_attribute_node_map(ctx, variable_node->attributes, pass);

    // We need a type for our symbol
    struct ZenitType *type = NULL;
    if (variable_node->type_decl)
    {
        // If the variable has a type hint, we should honor it
        type = get_type_from_type_declaration(ctx, variable_node->type_decl, rhs_symbol ? rhs_symbol->type : NULL);
    }
    else if (rhs_symbol)
    {
        // If the variable does not contain a type hint, we take the type from the 
        // right-hand side
        type = zenit_type_ctx_copy_type(ctx->types, rhs_symbol->type);
    }
    else
    {
        // This means the variable does not contain a type hint and the rhs does not
        // exist, which means there are errors out there, but we need to continue somehow
        type = zenit_type_ctx_new_none(ctx->types);
    }

    struct ZenitSymbol *symbol = zenit_symbol_new(variable_node->name, type);

    // Create and insert the symbol in the table
    return zenit_program_add_symbol(ctx->program, symbol);
}

/*
 * Function: visit_node
 *  This function selects the visitor function based on the node's type
 *  and calls the function.
 *
 * Parameters:
 *  ctx: Context object
 *  <struct ZenitNode> *node: Node to visit
 *  <enum ResolvePass> pass: The current resolve pass
 * 
 * Returns:
 *  struct ZenitSymbol* - A valid symbol if the visited node (or nodes) defines one or NULL
 *
 */
static struct ZenitSymbol* visit_node(struct ZenitContext *ctx, struct ZenitNode *node, enum ResolvePass pass)
{
    return symbol_resolvers[node->nodekind](ctx, node, pass);
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

    // NOTE: To simplify the resolve phase we split it in two passes
    // (by now), in the first pass we resolve user-defined types
    // and the second pass register all the symbols, including variables,
    // struct members, etc
    for (size_t i=0; i < fl_array_length(ctx->ast->decls); i++)
        visit_node(ctx, ctx->ast->decls[i], RESOLVE_STRUCT);

    for (size_t i=0; i < fl_array_length(ctx->ast->decls); i++)
        visit_node(ctx, ctx->ast->decls[i], RESOLVE_ALL);

    return errors == zenit_context_error_count(ctx);
}
