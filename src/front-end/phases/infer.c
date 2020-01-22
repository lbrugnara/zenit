#include "infer.h"
#include "utils.h"
#include "../program.h"
#include "../symbol.h"
#include "../types/context.h"

enum InferenceKind {
    INFER_NONE,
    INFER_UNIDIRECTIONAL,
    INFER_BIDIRECTIONAL,
};

typedef struct ZenitSymbol*(*ZenitTypeInferrer)(struct ZenitContext *ctx, struct ZenitNode *node, struct ZenitType **ctx_type, enum InferenceKind infer_kind);

// Visitor functions
static struct ZenitSymbol* visit_node(struct ZenitContext *ctx, struct ZenitNode *node, struct ZenitType **ctx_type, enum InferenceKind infer_kind);
static struct ZenitSymbol* visit_uint_node(struct ZenitContext *ctx, struct ZenitUintNode *node, struct ZenitType **ctx_type, enum InferenceKind infer_kind);
static struct ZenitSymbol* visit_variable_node(struct ZenitContext *ctx, struct ZenitVariableNode *variable_node, struct ZenitType **ctx_type, enum InferenceKind infer_kind);
static struct ZenitSymbol* visit_array_node(struct ZenitContext *ctx, struct ZenitArrayNode *array_node, struct ZenitType **ctx_type, enum InferenceKind infer_kind);
static struct ZenitSymbol* visit_identifier_node(struct ZenitContext *ctx, struct ZenitIdentifierNode *node, struct ZenitType **ctx_type, enum InferenceKind infer_kind);
static struct ZenitSymbol* visit_reference_node(struct ZenitContext *ctx, struct ZenitReferenceNode *reference_node, struct ZenitType **ctx_type, enum InferenceKind infer_kind);
static struct ZenitSymbol* visit_cast_node(struct ZenitContext *ctx, struct ZenitCastNode *cast_node, struct ZenitType **ctx_type, enum InferenceKind infer_kind);
static struct ZenitSymbol* visit_field_decl_node(struct ZenitContext *ctx, struct ZenitFieldDeclNode *field_node, struct ZenitType **ctx_type, enum InferenceKind infer_kind);
static struct ZenitSymbol* visit_struct_decl_node(struct ZenitContext *ctx, struct ZenitStructDeclNode *struct_node, struct ZenitType **ctx_type, enum InferenceKind infer_kind);
static struct ZenitSymbol* visit_struct_node(struct ZenitContext *ctx, struct ZenitStructNode *struct_node, struct ZenitType **ctx_type, enum InferenceKind infer_kind);

/*
 * Variable: inferrers
 *  An array indexed with a <enum ZenitNodeKind> to get a <ZenitTypeInferrer> function
 */
static const ZenitTypeInferrer inferrers[] = {
    [ZENIT_NODE_UINT]           = (ZenitTypeInferrer) &visit_uint_node,
    [ZENIT_NODE_VARIABLE]       = (ZenitTypeInferrer) &visit_variable_node,
    [ZENIT_NODE_ARRAY]          = (ZenitTypeInferrer) &visit_array_node,
    [ZENIT_NODE_IDENTIFIER]     = (ZenitTypeInferrer) &visit_identifier_node,
    [ZENIT_NODE_REFERENCE]      = (ZenitTypeInferrer) &visit_reference_node,
    [ZENIT_NODE_CAST]           = (ZenitTypeInferrer) &visit_cast_node,
    [ZENIT_NODE_FIELD_DECL]     = (ZenitTypeInferrer) &visit_field_decl_node,
    [ZENIT_NODE_STRUCT_DECL]    = (ZenitTypeInferrer) &visit_struct_decl_node,
    [ZENIT_NODE_STRUCT]         = (ZenitTypeInferrer) &visit_struct_node,
};

enum ZenitTypeUnifyResult {
    ZENIT_TYPE_UNIFY_NONE,
    ZENIT_TYPE_UNIFY_LEFT,
    ZENIT_TYPE_UNIFY_RIGHT,
    ZENIT_TYPE_UNIFY_BOTH
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
 *
 */
static struct ZenitSymbol* visit_cast_node(struct ZenitContext *ctx, struct ZenitCastNode *cast_node, struct ZenitType **ctx_type, enum InferenceKind infer_kind)
{
    struct ZenitSymbol *cast_symbol = zenit_utils_get_tmp_symbol(ctx->program, (struct ZenitNode*) cast_node);
    struct ZenitSymbol *casted_expression = visit_node(ctx, cast_node->expression, NULL, INFER_NONE);

    bool missing_cast_type = (ctx_type != NULL && cast_symbol->type->typekind == ZENIT_TYPE_NONE && (*ctx_type)->typekind == ZENIT_TYPE_NONE)
                                || (ctx_type == NULL && cast_symbol->type->typekind == ZENIT_TYPE_NONE);
    bool infer_dead_end = (infer_kind == INFER_NONE && cast_symbol->type->typekind == ZENIT_TYPE_NONE) 
                            || (ctx_type != NULL && infer_kind != INFER_BIDIRECTIONAL && (*ctx_type)->typekind == ZENIT_TYPE_NONE);

    if (missing_cast_type || infer_dead_end)
    {
        zenit_context_error(
            ctx, 
            cast_node->base.location, 
            ZENIT_ERROR_INFERENCE, 
            "Cannot infer the type of the cast expression, try adding a type hint to the cast expression"
        );
    }
    else if (ctx_type != NULL && infer_kind != INFER_NONE && zenit_type_can_unify(cast_symbol->type, *ctx_type) && !zenit_type_equals(cast_symbol->type, *ctx_type))
    {
        struct ZenitType *unified_type = NULL;
        if (zenit_type_ctx_unify_types(ctx->types, cast_symbol->type, *ctx_type, &unified_type))
        {
            if (!zenit_type_equals(cast_symbol->type, unified_type))
            {
                cast_symbol->type = zenit_type_ctx_copy_type(ctx->types, unified_type);
            }

            if (infer_kind == INFER_BIDIRECTIONAL)
                *ctx_type = zenit_type_ctx_copy_type(ctx->types, unified_type);
        }
    }

    return cast_symbol;
}

/*
 * Function: visit_uint_node
 *  A literal node always has a type therefore we just return it
 *
 * Parameters:
 *  ctx - Context object
 *  node - Literal node
 *
 * Returns:
 *
 */
static struct ZenitSymbol* visit_uint_node(struct ZenitContext *ctx, struct ZenitUintNode *node, struct ZenitType **ctx_type, enum InferenceKind infer_kind)
{
    struct ZenitSymbol *uint_symbol = zenit_utils_get_tmp_symbol(ctx->program, (struct ZenitNode*) node);

    if (ctx_type != NULL && infer_kind == INFER_BIDIRECTIONAL 
        && zenit_type_can_unify(uint_symbol->type, *ctx_type) 
        && !zenit_type_equals(uint_symbol->type, *ctx_type))
    {
        struct ZenitType *unified_type = NULL;
        if (zenit_type_ctx_unify_types(ctx->types, uint_symbol->type, *ctx_type, &unified_type) && !zenit_type_equals(*ctx_type, unified_type))
            *ctx_type = zenit_type_ctx_copy_type(ctx->types, unified_type);
    }

    return uint_symbol;
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
 *
 */
static struct ZenitSymbol* visit_identifier_node(struct ZenitContext *ctx, struct ZenitIdentifierNode *node, struct ZenitType **ctx_type, enum InferenceKind infer_kind)
{
    struct ZenitSymbol *id_symbol = zenit_program_get_symbol(ctx->program, node->name);

    // TODO: Check if it is ok to ignore the contextual type information for the symbol
    if (ctx_type != NULL && infer_kind == INFER_BIDIRECTIONAL 
        && zenit_type_can_unify(id_symbol->type, *ctx_type) 
        && !zenit_type_equals(id_symbol->type, *ctx_type))
    {
        struct ZenitType *unified_type = NULL;
        if (zenit_type_ctx_unify_types(ctx->types, id_symbol->type, *ctx_type, &unified_type) && !zenit_type_equals(*ctx_type, unified_type))
            *ctx_type = zenit_type_ctx_copy_type(ctx->types, unified_type);
    }

    return id_symbol;
}

/*
 * Function: visit_reference_node
 *  In this function we need to fill the type information of the <struct ZenitReferenceNode> object using
 *  the expression type object.
 *
 * Parameters:
 *  ctx - Context object
 *  node - Array initializer node
 *
 * Returns:
 *
 */
static struct ZenitSymbol* visit_reference_node(struct ZenitContext *ctx, struct ZenitReferenceNode *reference_node, struct ZenitType **ctx_type, enum InferenceKind infer_kind)
{
    struct ZenitSymbol *ref_symbol = zenit_utils_get_tmp_symbol(ctx->program, (struct ZenitNode*) reference_node);

    visit_node(ctx, reference_node->expression, NULL, INFER_NONE);

    // TODO: Check if it is ok to ignore the contextual type information for the reference symbol
    if (ctx_type != NULL && infer_kind == INFER_BIDIRECTIONAL 
        && zenit_type_can_unify(ref_symbol->type, *ctx_type) 
        && !zenit_type_equals(ref_symbol->type, *ctx_type))
    {
        struct ZenitType *unified_type = NULL;
        if (zenit_type_ctx_unify_types(ctx->types, ref_symbol->type, *ctx_type, &unified_type) && !zenit_type_equals(*ctx_type, unified_type))
            *ctx_type = zenit_type_ctx_copy_type(ctx->types, unified_type);
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
 *
 */
static struct ZenitSymbol* visit_array_node(struct ZenitContext *ctx, struct ZenitArrayNode *array_node, struct ZenitType **ctx_type, enum InferenceKind infer_kind)
{
    struct ZenitSymbol *array_symbol = zenit_utils_get_tmp_symbol(ctx->program, (struct ZenitNode*) array_node);

    // If we received a type hint from the calling context, we can use it to update our array's member type,
    // and we can also use it with every element within the array
    // If the typehint is not null, at the end of this check, we will get a "member" type hint
    enum InferenceKind member_infer_kind = INFER_NONE;
    if (ctx_type != NULL && infer_kind != INFER_NONE)
    {
        bool ctx_type_is_array = (*ctx_type)->typekind == ZENIT_TYPE_ARRAY;

        if (ctx_type_is_array)
        {
            // If the ctx_type is an array type too, we can unify it with our -so far- inferred
            // array type
            if (zenit_type_can_unify(array_symbol->type, *ctx_type))
            {
                if (!zenit_type_equals(array_symbol->type, *ctx_type))
                {
                    struct ZenitType *unified_type = NULL;
                    if (zenit_type_ctx_unify_types(ctx->types, array_symbol->type, *ctx_type, &unified_type))
                    {
                        if (!zenit_type_equals(array_symbol->type, unified_type))
                        {
                            array_symbol->type = zenit_type_ctx_copy_type(ctx->types, unified_type);
                        }

                        if (infer_kind == INFER_BIDIRECTIONAL && !zenit_type_equals(*ctx_type, unified_type))
                            *ctx_type = zenit_type_ctx_copy_type(ctx->types, unified_type);
                    }
                }
            }
            else
            {
                // If the array types cannot be unified, it could be because of a different length between the arrays, so 
                // let's try directly with their member types
                struct ZenitArrayType *array_type = (struct ZenitArrayType*) array_symbol->type;
                struct ZenitArrayType *ctx_array_type = (struct ZenitArrayType*) *ctx_type;

                if (zenit_type_can_unify(array_type->member_type, ctx_array_type->member_type))
                {
                    if (!zenit_type_equals(array_type->member_type, ctx_array_type->member_type))
                    {
                        struct ZenitType *unified_type = NULL;
                        if (zenit_type_ctx_unify_types(ctx->types, array_type->member_type, ctx_array_type->member_type, &unified_type))
                        {
                            if (!zenit_type_equals(array_type->member_type, unified_type))
                            {
                                array_type->member_type = zenit_type_ctx_copy_type(ctx->types, unified_type);
                            }

                            if (infer_kind == INFER_BIDIRECTIONAL && !zenit_type_equals(ctx_array_type->member_type, unified_type))
                                ctx_array_type->member_type = zenit_type_ctx_copy_type(ctx->types, unified_type);
                        }
                    }
                }
            }

            // If the contextual type can be inferred from the array, the same will happen with the array member's type
            // (and therefore with the whole array type)
            member_infer_kind = infer_kind;
        }
        else
        {
            // If the hint type is not an array, we don't have much more information, we just allow bidirectional inference
            // and hope for the best (?)
            member_infer_kind = INFER_BIDIRECTIONAL;
        }
    }    

    struct ZenitArrayType *array_type = (struct ZenitArrayType*) array_symbol->type;

    // Visit each element node and try to unify each element type with the array's member type
    for (size_t i=0; i < fl_array_length(array_node->elements); i++)
    {
        // If we received a type hint, and we used it to infer the array type, we can pass the member_type property as a hint to the
        // array element visitor
        struct ZenitSymbol *elem_symbol = visit_node(ctx, 
                                                     array_node->elements[i], 
                                                     &((struct ZenitArrayType*) array_symbol->type)->member_type, 
                                                     member_infer_kind);
    }

    if (ctx_type != NULL && infer_kind == INFER_BIDIRECTIONAL 
        && zenit_type_can_unify(array_symbol->type, *ctx_type) 
        && !zenit_type_equals(array_symbol->type, *ctx_type))
    {
        struct ZenitType *unified_type = NULL;
        if (zenit_type_ctx_unify_types(ctx->types, array_symbol->type, *ctx_type, &unified_type))
        {
            if (!zenit_type_equals(array_symbol->type, unified_type))
            {
                array_symbol->type = zenit_type_ctx_copy_type(ctx->types, unified_type);
            }

            if (!zenit_type_equals(*ctx_type, unified_type))
                *ctx_type = zenit_type_ctx_copy_type(ctx->types, unified_type);
        }
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
            struct ZenitSymbol *value_symbol = visit_node(ctx, prop->value, NULL, INFER_NONE);

            // FIXME: Properties do not have types -so far-
        }

        fl_array_free(properties);
    }

    fl_array_free(names);
}


static struct ZenitSymbol* visit_struct_node(struct ZenitContext *ctx, struct ZenitStructNode *struct_node, struct ZenitType **ctx_type, enum InferenceKind infer_kind)
{
    struct ZenitSymbol *struct_symbol = zenit_utils_get_tmp_symbol(ctx->program, (struct ZenitNode*) struct_node);

    const char *struct_name = struct_node->name;

    if (struct_node->name == NULL && ctx_type != NULL && (*ctx_type)->typekind == ZENIT_TYPE_STRUCT)
    {
        // If the struct name is NULL, it is an unnamed struct, but because we have type information from the calling context
        // we can check if the type is a named struct, and in that case we use that information here.
        // NOTE: We are using the contextual type information just to pass it to the struct's members, we DON'T use it to
        // infer the struct type. Unnamed structs are "promoted" to named ones through implicit casting (if possible).
        // (check the variable node visitor for an example)
        struct ZenitStructType *hint_struct = (struct ZenitStructType*) *ctx_type;
        struct_name = hint_struct->name;
    }

    struct ZenitScope *struct_scope = NULL;
    
    if (struct_name != NULL)
        struct_scope = zenit_program_get_scope(ctx->program, ZENIT_SCOPE_STRUCT, struct_name);

    for (size_t i=0; i < fl_array_length(struct_node->members); i++)
    {
        if (struct_node->members[i]->nodekind == ZENIT_NODE_FIELD)
        {
            struct ZenitFieldNode *field_node = (struct ZenitFieldNode*) struct_node->members[i];

            struct ZenitSymbol *field_decl_symbol = struct_scope != NULL ? zenit_scope_get_symbol(struct_scope, field_node->name) : NULL;

            struct ZenitSymbol *value_symbol = NULL;
            
            if (field_decl_symbol != NULL)
                value_symbol = visit_node(ctx, field_node->value, &field_decl_symbol->type, INFER_UNIDIRECTIONAL);
            else
                value_symbol = visit_node(ctx, field_node->value, NULL, INFER_NONE);
            
        }
    }

    if (ctx_type != NULL && infer_kind == INFER_BIDIRECTIONAL 
        && zenit_type_can_unify(struct_symbol->type, *ctx_type) 
        && !zenit_type_equals(struct_symbol->type, *ctx_type))
    {
        struct ZenitType *unified_type = NULL;
        if (zenit_type_ctx_unify_types(ctx->types, struct_symbol->type, *ctx_type, &unified_type))
        {
            if (!zenit_type_equals(struct_symbol->type, unified_type))
            {
                struct_symbol->type = zenit_type_ctx_copy_type(ctx->types, unified_type);
            }

            if (!zenit_type_equals(*ctx_type, unified_type))
                *ctx_type = zenit_type_ctx_copy_type(ctx->types, unified_type);
        }
    }

    return struct_symbol;
}

static struct ZenitSymbol* visit_field_decl_node(struct ZenitContext *ctx, struct ZenitFieldDeclNode *field_node, struct ZenitType **ctx_type, enum InferenceKind infer_kind)
{
    return zenit_program_get_symbol(ctx->program, field_node->name);
}

static struct ZenitSymbol* visit_struct_decl_node(struct ZenitContext *ctx, struct ZenitStructDeclNode *struct_node, struct ZenitType **ctx_type, enum InferenceKind infer_kind)
{
    // Visit the attributes and its properties
    visit_attribute_node_map(ctx, &struct_node->attributes);

    zenit_program_push_scope(ctx->program, ZENIT_SCOPE_STRUCT, struct_node->name);

    for (size_t i=0; i < fl_array_length(struct_node->members); i++)
        visit_node(ctx, struct_node->members[i], NULL, INFER_NONE);

    zenit_program_pop_scope(ctx->program);

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
 *
 */
static struct ZenitSymbol* visit_variable_node(struct ZenitContext *ctx, struct ZenitVariableNode *variable_node, struct ZenitType **ctx_type, enum InferenceKind infer_kind)
{
    // The variable declaration node does not perform type inference in this function, but it phases type information
    // to the right-hand side expression's visitor with possibly INFER_BIDIRECTIONAL when the type is not present in the
    // declaration:
    //  e.g.: var a = [ 1, 2, 3 ]
    //  The INFER_BIDIRECTIONAL is passed to the array's visitor, which means that the type from the array literal will be assigned
    //  to the variable's type
    flm_assert(ctx_type == NULL && infer_kind == INFER_NONE, "A variable declaration does not need contextual type information");

    // Visit the attributes and its properties
    visit_attribute_node_map(ctx, &variable_node->attributes);

    // We need the symbol we introduced in the <zenit_resolve_symbols> pass
    struct ZenitSymbol *symbol = zenit_program_get_symbol(ctx->program, variable_node->name);

    // We need to get the symbol of the right-hand side expression. (if the variable definition has a type hint, we pass that hint to the visitor)
    struct ZenitSymbol *rhs_symbol = visit_node(ctx, 
                                                variable_node->rvalue, 
                                                &symbol->type, 
                                                // If the variable has a declared type, we don't allow inference on the variable
                                                variable_node->type_decl != NULL
                                                    ? INFER_UNIDIRECTIONAL 
                                                    : INFER_BIDIRECTIONAL);

    // If the types are not equals, we try to cast the RHS to the LHS type, and if that is not possible, we don't do anything here, we just let
    // the type check to fail later.
    if (!zenit_type_equals(symbol->type, rhs_symbol->type) && zenit_type_is_castable_to(rhs_symbol->type, symbol->type))
    {   
        // NOTE: the ZenitVariableNode structure changes, but we don't need to worry about its UID changing because of that
        // as the variables are always accessed by its name, and not by the UID
        struct ZenitCastNode *cast_node = zenit_node_cast_new(variable_node->rvalue->location, variable_node->rvalue, true);
        variable_node->rvalue = (struct ZenitNode*) cast_node;

        zenit_utils_new_tmp_symbol(ctx->program, (struct ZenitNode*) cast_node, zenit_type_ctx_copy_type(ctx->types, symbol->type));
    }

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
 *
 */
static struct ZenitSymbol* visit_node(struct ZenitContext *ctx, struct ZenitNode *node, struct ZenitType **ctx_type, enum InferenceKind infer_kind)
{
    return inferrers[node->nodekind](ctx, node, ctx_type, infer_kind);
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
        visit_node(ctx, ctx->ast->decls[i], NULL, INFER_NONE);

    return errors == zenit_context_error_count(ctx);
}
