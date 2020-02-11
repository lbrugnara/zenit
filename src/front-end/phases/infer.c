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
static struct ZenitSymbol* visit_uint_node(struct ZenitContext *ctx, struct ZenitUintNode *uint_node, struct ZenitType **ctx_type, enum InferenceKind infer_kind);
static struct ZenitSymbol* visit_bool_node(struct ZenitContext *ctx, struct ZenitBoolNode *bool_node, struct ZenitType **ctx_type, enum InferenceKind infer_kind);
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
    [ZENIT_NODE_BOOL]           = (ZenitTypeInferrer) &visit_bool_node,
    [ZENIT_NODE_VARIABLE]       = (ZenitTypeInferrer) &visit_variable_node,
    [ZENIT_NODE_ARRAY]          = (ZenitTypeInferrer) &visit_array_node,
    [ZENIT_NODE_IDENTIFIER]     = (ZenitTypeInferrer) &visit_identifier_node,
    [ZENIT_NODE_REFERENCE]      = (ZenitTypeInferrer) &visit_reference_node,
    [ZENIT_NODE_CAST]           = (ZenitTypeInferrer) &visit_cast_node,
    [ZENIT_NODE_FIELD_DECL]     = (ZenitTypeInferrer) &visit_field_decl_node,
    [ZENIT_NODE_STRUCT_DECL]    = (ZenitTypeInferrer) &visit_struct_decl_node,
    [ZENIT_NODE_STRUCT]         = (ZenitTypeInferrer) &visit_struct_node,
};

enum UnificationKind {
    UNIFY_NONE,
    UNIFY_A,
    UNIFY_B,
    UNIFY_ALL
};

static inline enum UnificationKind iktouk(enum InferenceKind infer_kind)
{
    switch (infer_kind)
    {
        case INFER_NONE:
            return UNIFY_NONE;

        case INFER_UNIDIRECTIONAL:
            return UNIFY_A;

        case INFER_BIDIRECTIONAL:
            return UNIFY_ALL;
    }

    return UNIFY_NONE;
}

/*
 * Function: try_type_unification
 *  This function receives two pointers to type objects and tries to find the common ancestor between the two types, and 
 *  in case it exists, this function based on the value of the *unification_kind* variable updates each type accordingly.
 *
 * Parameters:
 *  <struct ZenitTypeContext> *type_ctx: Type context object
 *  <enum UnificationKind> unification_kind: Tells the function which type it needs to update
 *  <struct ZenitType> **type_a: Pointer to a type object. If *unification_kind* is UNIFY_A or UNIFY_ALL, the function updates the type if there
 *                               is a common ancestor between the types
 *  <struct ZenitType> **type_b: Pointer to a type object. If *unification_kind* is UNIFY_B or UNIFY_ALL, the function updates the type if there
 *                               is a common ancestor between the types
 *
 * Returns:
 *  void: This function does not return a value
 */
static inline void try_type_unification(struct ZenitTypeContext *type_ctx, enum UnificationKind unification_kind, struct ZenitType **type_a, struct ZenitType **type_b)
{
    if (unification_kind == UNIFY_NONE)
        return;

    if (zenit_type_can_unify(*type_a, *type_b) && !zenit_type_equals(*type_a, *type_b))
    {
        struct ZenitType *unified_type = NULL;
        if (zenit_type_ctx_unify_types(type_ctx, *type_a, *type_b, &unified_type))
        {
            if ((unification_kind == UNIFY_A || unification_kind == UNIFY_ALL) && !zenit_type_equals(*type_a, unified_type))
                *type_a = zenit_type_ctx_copy_type(type_ctx, unified_type);

            if ((unification_kind == UNIFY_B || unification_kind == UNIFY_ALL) && !zenit_type_equals(*type_b, unified_type))
                *type_b = zenit_type_ctx_copy_type(type_ctx, unified_type);
        }
    }
}

/*
 * Function: visit_cast_node
 *  The cast expression does not infer anything, because first it honors the casted expression's type,
 *  and second, the cast's type is explicit "cast(0x1FF : uint8)" or inferred by another node (like a
 *  variable declaration: "var a : uint8 = cast(0x1FF)"  )
 * 
 * Parameters:
 *  <struct ZenitContext> *ctx - Context object
 *  <struct ZenitCastNode> *cast_node - Cast node
 *  <struct ZenitType> *ctx_type: Contextual type information
 *  <enum InferenceKind> *infer_kind: Contextual information about the inference process
 *
 * Returns:
 *  <struct ZenitSymbol>*: The temporal symbol of the cast expression
 */
static struct ZenitSymbol* visit_cast_node(struct ZenitContext *ctx, struct ZenitCastNode *cast_node, struct ZenitType **ctx_type, enum InferenceKind infer_kind)
{
    struct ZenitSymbol *cast_symbol = zenit_utils_get_tmp_symbol(ctx->program, (struct ZenitNode*) cast_node);
    struct ZenitSymbol *casted_expression = visit_node(ctx, cast_node->expression, NULL, INFER_NONE);

    // NOTE: If the type of the cast is NONE, it means the cast does not contain type information. In that case it is mandatory for the contextual
    // type information to be present and valid, because if not, we can't infer the cast type
    bool missing_cast_type = (ctx_type != NULL && cast_symbol->type->typekind == ZENIT_TYPE_NONE && (*ctx_type)->typekind == ZENIT_TYPE_NONE)
                                || (ctx_type == NULL && cast_symbol->type->typekind == ZENIT_TYPE_NONE);

    // NOTE: If the infer_kind is NONE and the cast does not contain type information, we can't update the cast's type and in that
    // case its type will remain NONE at the end of the infer pass, which is something we don't want.
    bool infer_dead_end = (infer_kind == INFER_NONE && cast_symbol->type->typekind == ZENIT_TYPE_NONE) 
                            || (ctx_type != NULL && infer_kind != INFER_BIDIRECTIONAL && (*ctx_type)->typekind == ZENIT_TYPE_NONE);

    if (missing_cast_type || infer_dead_end)
    {
        // If one of the conditions above is true, it means we can't infer the type, so let's add an error and leave
        zenit_context_error(
            ctx, 
            cast_node->base.location, 
            ZENIT_ERROR_INFERENCE, 
            "Cannot infer the type of the cast expression, try adding a type hint to the cast expression"
        );
        return cast_symbol;
    }

    // At this point it safe to try the type unification
    try_type_unification(ctx->types, iktouk(infer_kind), &cast_symbol->type, ctx_type);

    return cast_symbol;
}

/*
 * Function: visit_uint_node
 *  A uint node always has a type therefore we just return it
 *
 * Parameters:
 *  <struct ZenitContext> *ctx - Context object
 *  <struct ZenitUintNode> *uint_node - Uint node
 *  <struct ZenitType> *ctx_type: Contextual type information
 *  <enum InferenceKind> *infer_kind: Contextual information about the inference process
 *
 * Returns:
 *  <struct ZenitSymbol>*: The temporal symbol of the uint literal
 */
static struct ZenitSymbol* visit_uint_node(struct ZenitContext *ctx, struct ZenitUintNode *uint_node, struct ZenitType **ctx_type, enum InferenceKind infer_kind)
{
    struct ZenitSymbol *uint_symbol = zenit_utils_get_tmp_symbol(ctx->program, (struct ZenitNode*) uint_node);

    // NOTE: The uint type is intrinsic, it can't be updated/modified, but the context might ask for type 
    // information, and in that case, we update just the ctx_type object
    if (ctx_type != NULL && infer_kind == INFER_BIDIRECTIONAL)
        try_type_unification(ctx->types, UNIFY_B, &uint_symbol->type, ctx_type);

    return uint_symbol;
}

/*
 * Function: visit_bool_node
 *  A bool node always has a type therefore we just return it
 *
 * Parameters:
 *  <struct ZenitContext> *ctx - Context object
 *  <struct ZenitBoolNode> *bool_node - Boolean node
 *  <struct ZenitType> *ctx_type: Contextual type information
 *  <enum InferenceKind> *infer_kind: Contextual information about the inference process
 *
 * Returns:
 *  <struct ZenitSymbol>*: The temporal symbol of the boolean literal
 */
static struct ZenitSymbol* visit_bool_node(struct ZenitContext *ctx, struct ZenitBoolNode *bool_node, struct ZenitType **ctx_type, enum InferenceKind infer_kind)
{
    struct ZenitSymbol *bool_symbol = zenit_utils_get_tmp_symbol(ctx->program, (struct ZenitNode*) bool_node);

    // NOTE: The bool type is intrinsic, it can't be updated/modified, but the context might ask for type 
    // information, and in that case, we update just the ctx_type object
    if (ctx_type != NULL && infer_kind == INFER_BIDIRECTIONAL)
        try_type_unification(ctx->types, UNIFY_B, &bool_symbol->type, ctx_type);

    return bool_symbol;
}

/*
 * Function: visit_identifier_node
 *  At this point the symbol must be defined, so we need to retrieve it from the symbol table
 *  and return its type information
 *
 * Parameters:
 *  <struct ZenitContext> *ctx - Context object
 *  <struct ZenitIdentifierNode> *id_node - Identifier node
 *  <struct ZenitType> *ctx_type: Contextual type information
 *  <enum InferenceKind> *infer_kind: Contextual information about the inference process
 * 
 * Returns:
 *  <struct ZenitSymbol>*: The symbol of the identifier
 */
static struct ZenitSymbol* visit_identifier_node(struct ZenitContext *ctx, struct ZenitIdentifierNode *id_node, struct ZenitType **ctx_type, enum InferenceKind infer_kind)
{
    struct ZenitSymbol *id_symbol = zenit_program_get_symbol(ctx->program, id_node->name);

    // NOTE: We don't infer the identifier type, because we assume the type information is provided/inferred on the
    // symbol declaration, but we do want to provide type information to the context if it request us that information
    if (ctx_type != NULL && infer_kind == INFER_BIDIRECTIONAL)
        try_type_unification(ctx->types, UNIFY_B, &id_symbol->type, ctx_type);

    return id_symbol;
}

/*
 * Function: visit_reference_node
 *  The reference visitor just visits (pun intended) the referenced expression. The type information of that expression is already present
 *  in the reference type (see the resolve pass), so we don't need to "unify" its types. What we can do is, if the context asks for type information
 *  (INFER_BIDIRECTIONAL) populates the *ctx_type* object with the reference type.
 *
 * Parameters:
 *  <struct ZenitContext> *ctx: Context object
 *  <struct ZenitReferenceNode> *reference_node: Node to visit
 *  <struct ZenitType> *ctx_type: Contextual type information
 *  <enum InferenceKind> *infer_kind: Contextual information about the inference process
 *
 * Returns:
 *  <struct ZenitSymbol>* - The reference symbol with its type information
 */
static struct ZenitSymbol* visit_reference_node(struct ZenitContext *ctx, struct ZenitReferenceNode *reference_node, struct ZenitType **ctx_type, enum InferenceKind infer_kind)
{
    struct ZenitSymbol *ref_symbol = zenit_utils_get_tmp_symbol(ctx->program, (struct ZenitNode*) reference_node);
    struct ZenitReferenceType *ref_type = (struct ZenitReferenceType*) ref_symbol->type;

    // The referenced element type is inferred (check the resolve pass) so it is ok to ask for bidirectional inference here
    visit_node(ctx, reference_node->expression, &ref_type->element, INFER_BIDIRECTIONAL);

    // Now it's time to check if we should update the reference type or update the contextual type
    try_type_unification(ctx->types, iktouk(infer_kind), &ref_symbol->type, ctx_type);

    return ref_symbol;
}

/*
 * Function: visit_array_node
 *  The array visitor infers the type of the array from its members if the context does not provide type information, or uses the type information
 *  it receives to come up with the array type.
 *
 * Parameters:
 *  <struct ZenitContext> *ctx: Context object
 *  <struct ZenitArrayNode> *array_node: Node to visit
 *  <struct ZenitType> *ctx_type: Contextual type information
 *  <enum InferenceKind> *infer_kind: Contextual information about the inference process
 *
 * Returns:
 *  <struct ZenitSymbol>* - The array symbol with its type information
 */
static struct ZenitSymbol* visit_array_node(struct ZenitContext *ctx, struct ZenitArrayNode *array_node, struct ZenitType **ctx_type, enum InferenceKind infer_kind)
{
    struct ZenitSymbol *array_symbol = zenit_utils_get_tmp_symbol(ctx->program, (struct ZenitNode*) array_node);

    // If we receive a type hint from the calling context, we can use it to update our array's member type,
    // and we can also use it for the array elements
    enum InferenceKind member_infer_kind = INFER_NONE;
    if (ctx_type != NULL && infer_kind != INFER_NONE)
    {
        bool ctx_type_is_array = (*ctx_type)->typekind == ZENIT_TYPE_ARRAY;

        if (ctx_type_is_array)
        {            
            if (zenit_type_can_unify(array_symbol->type, *ctx_type))
            {
                try_type_unification(ctx->types, iktouk(infer_kind), &array_symbol->type, ctx_type);
            }
            else
            {
                // If the array types cannot be unified, it could be because of a different length between the arrays, so 
                // let's try directly with their member types
                struct ZenitArrayType *array_type = (struct ZenitArrayType*) array_symbol->type;
                struct ZenitArrayType *ctx_array_type = (struct ZenitArrayType*) *ctx_type;

                if (zenit_type_can_unify(array_type->member_type, ctx_array_type->member_type))
                    try_type_unification(ctx->types, iktouk(infer_kind), &array_type->member_type, &ctx_array_type->member_type);
            }

            // We directly assign the infer_kind of the context to the member_infer_kind because:
            //  - If the context sent INFER_UNIDIRECTIONAL it means there is a type hint out there in the context, and we should honor that
            //  - If the context sent INFER_BIDIRECTIONAL, we want the member visitors to be able to infer the array type and also the context type
            //    if possible
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

    struct ZenitSymbol **elements = fl_array_new(sizeof(struct ZenitSymbol*), fl_array_length(array_node->elements));

    // Visit each element node and try to unify each element type with the array's member type
    for (size_t i=0; i < fl_array_length(array_node->elements); i++)
    {
        struct ZenitSymbol *elem_symbol = visit_node(ctx, 
                                                     array_node->elements[i], 
                                                     // We directly pass the array member type
                                                     &((struct ZenitArrayType*) array_symbol->type)->member_type, 
                                                     member_infer_kind);

        elements[i] = elem_symbol;
    }

    for (size_t i=0; i < fl_array_length(elements); i++)
    {
        // If the types are not equals, we try to cast the RHS to the LHS type, and if that is not possible, we don't do anything here, we just let
        // the type check to fail later.
        if (!zenit_type_equals(array_type->member_type, elements[i]->type) && zenit_type_is_castable_to(elements[i]->type, array_type->member_type))
        {   
            // NOTE: the ZenitVariableNode structure changes, but we don't need to worry about its UID changing because of that
            // as the variables are always accessed by its name, and not by the UID
            struct ZenitCastNode *cast_node = zenit_node_cast_new(array_node->elements[i]->location, array_node->elements[i], true);
            array_node->elements[i] = (struct ZenitNode*) cast_node;

            zenit_utils_new_tmp_symbol(ctx->program, (struct ZenitNode*) cast_node, zenit_type_ctx_copy_type(ctx->types, array_type->member_type));
        }
    }

    fl_array_free(elements);

    // If the context is waiting for type information (BIDIRECTIONAL) we need to check the array's member type, because we could
    // have updated it when we visited the array elements, and that type information must be shared with the caller:
    //  e.g.:
    //      var a = [ 1, 2, 0x1FF ];
    //
    //  Here the variable declaration pass INFER_BIDIRECTIONAL, and the last element in the array promotes the array member type to
    //  uint16. At this point in the execution, the type of the array is [3]uint16 and we should fill that information in the ctx_type
    //  pointer which is the variable's type information
    try_type_unification(ctx->types, iktouk(infer_kind), &array_symbol->type, ctx_type);

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
 *  By now, this function is pretty dumb because there is nothing to infer in the
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
            struct ZenitSymbol *value_symbol = visit_node(ctx, prop->value, NULL, INFER_NONE);

            // FIXME: Properties do not have types -so far-
        }

        fl_array_free(properties);
    }

    fl_array_free(names);
}

/*
 * Function: visit_named_struct_node
 *  The struct literal visitor iterate over the struct fields and tries to provide contextual type information for them. This visitor doesn't
 *  need to infer anything, as all the type information should be present on the struct declaration, but the type of its fields are used to infer 
 *  the type of the expressions that are used to initialize them.
 *
 * Parameters:
 *  <struct ZenitContext> *ctx: Context object
 *  <struct ZenitStructNode> *struct_node: Node to visit
 *  <struct ZenitType> *ctx_type: Contextual type information
 *  <enum InferenceKind> *infer_kind: Contextual information about the inference process
 *
 * Returns:
 *  <struct ZenitSymbol>* - The struct symbol with its type information
 */
static struct ZenitSymbol* visit_named_struct_node(struct ZenitContext *ctx, struct ZenitStructNode *struct_node, struct ZenitType **ctx_type, enum InferenceKind infer_kind)
{
    struct ZenitSymbol *struct_symbol = zenit_utils_get_tmp_symbol(ctx->program, (struct ZenitNode*) struct_node);

    // We need the named struct's scope to get type information from its members
    struct ZenitScope *struct_scope = zenit_program_get_scope(ctx->program, ZENIT_SCOPE_STRUCT, struct_node->name);

    for (size_t i=0; i < fl_array_length(struct_node->members); i++)
    {
        if (struct_node->members[i]->nodekind == ZENIT_NODE_FIELD)
        {
            struct ZenitFieldNode *field_node = (struct ZenitFieldNode*) struct_node->members[i];

            // We retrieve the field declaration to pass it ot the field value's visitor
            struct ZenitSymbol *field_decl_symbol = zenit_scope_get_symbol(struct_scope, field_node->name);

            // Send the declared type information and disallow inference over the field's type
            visit_node(ctx, field_node->value, &field_decl_symbol->type, INFER_UNIDIRECTIONAL);
        }
    }

    return struct_symbol;
}

/*
 * Function: visit_unnamed_struct_node
 *  For unnamed structs, we don't have type information for the initializers, unless we receive type information from the context:
 *      e.g.:
 *          struct Point { x: uint8; y: uint8; }
 *          var p : Point = { x: 1, y: 2 };
 * 
 *      In this statement, we receive the contextual type Point from the variable declaration, with that information we can use the fields of
 *      the Point struct to pass contextual type information to the struct literal fields (uint8)
 *
 * Parameters:
 *  <struct ZenitContext> *ctx: Context object
 *  <struct ZenitStructNode> *struct_node: Node to visit
 *  <struct ZenitType> *ctx_type: Contextual type information
 *  <enum InferenceKind> *infer_kind: Contextual information about the inference process
 *
 * Returns:
 *  <struct ZenitSymbol>* - The struct symbol with its type information
 */
static struct ZenitSymbol* visit_unnamed_struct_node(struct ZenitContext *ctx, struct ZenitStructNode *struct_node, struct ZenitType **ctx_type, enum InferenceKind infer_kind)
{
    struct ZenitSymbol *struct_symbol = zenit_utils_get_tmp_symbol(ctx->program, (struct ZenitNode*) struct_node);

    // The struct name can help us to infer the type of the struct members and its initializers for unnamed struct 
    // or to infer the type of the initializers for named struct members.
    const char *struct_name = struct_node->name;

    if (struct_node->name == NULL && ctx_type != NULL && (*ctx_type)->typekind == ZENIT_TYPE_STRUCT)
    {
        // If the struct name is NULL, it is an unnamed struct, but because we have type information from the calling context
        // we can check if the type is a named struct, and in that case we use that information here.
        // NOTE: We are using the contextual type information just to pass it to the struct's members, we DON'T use it to
        // infer the struct type. Unnamed structs are "promoted" to named ones through implicit casting (if possible).
        // (check the variable node visitor for an example)
        struct_name = ((struct ZenitStructType*) *ctx_type)->name;
    }

    // If we have a struct name, we must also have a scope
    struct ZenitScope *struct_scope = NULL;
    
    if (struct_name != NULL)
        struct_scope = zenit_program_get_scope(ctx->program, ZENIT_SCOPE_STRUCT, struct_name);

    for (size_t i=0; i < fl_array_length(struct_node->members); i++)
    {
        if (struct_node->members[i]->nodekind == ZENIT_NODE_FIELD)
        {
            struct ZenitFieldNode *field_node = (struct ZenitFieldNode*) struct_node->members[i];
            struct ZenitStructTypeMember *field_member = zenit_type_struct_get_member((struct ZenitStructType*) struct_symbol->type, field_node->name);

            if (struct_name != NULL)
            {
                // If the struct name is available from the context, we use it to update the member type information
                struct ZenitSymbol *field_decl_symbol = zenit_scope_get_symbol(struct_scope, field_node->name);

                try_type_unification(ctx->types, UNIFY_A, &field_member->type, &field_decl_symbol->type);
            }

            // For unnamed structs we allow bidirectional inference. We don't care about the type hint,
            // it can help us, but it is not mandatory
            visit_node(ctx, field_node->value, &field_member->type, INFER_BIDIRECTIONAL);
        }
    }

    return struct_symbol;
}

/*
 * Function: visit_struct_node
 *  This visitor calls the specific visitor for named and unnamed structs, and after that tries to make a unification between the struct type
 *  and the contextual type information if needed.
 *
 * Parameters:
 *  <struct ZenitContext> *ctx: Context object
 *  <struct ZenitStructNode> *struct_node: Node to visit
 *  <struct ZenitType> *ctx_type: Contextual type information
 *  <enum InferenceKind> *infer_kind: Contextual information about the inference process
 *
 * Returns:
 *  <struct ZenitSymbol>* - The struct symbol with its type information
 */
static struct ZenitSymbol* visit_struct_node(struct ZenitContext *ctx, struct ZenitStructNode *struct_node, struct ZenitType **ctx_type, enum InferenceKind infer_kind)
{
    struct ZenitSymbol *struct_symbol = NULL;

    if (struct_node->name != NULL)
        struct_symbol = visit_named_struct_node(ctx, struct_node, ctx_type, infer_kind);
    else
        struct_symbol = visit_unnamed_struct_node(ctx, struct_node, ctx_type, infer_kind);

    try_type_unification(ctx->types, iktouk(infer_kind), &struct_symbol->type, ctx_type);

    return struct_symbol;
}

/*
 * Function: visit_field_decl_node
 *  The field declaration visitor returns the field symbol, as its type information is always present in the declaration
 *
 * Parameters:
 *  <struct ZenitContext> *ctx: Context object
 *  <struct ZenitFieldDeclNode> *field_node: Node to visit
 *  <struct ZenitType> *ctx_type: Contextual type information
 *  <enum InferenceKind> *infer_kind: Contextual information about the inference process
 *
 * Returns:
 *  <struct ZenitSymbol>* - The field symbol with its type information
 */
static struct ZenitSymbol* visit_field_decl_node(struct ZenitContext *ctx, struct ZenitFieldDeclNode *field_node, struct ZenitType **ctx_type, enum InferenceKind infer_kind)
{
    return zenit_program_get_symbol(ctx->program, field_node->name);
}

/*
 * Function: visit_struct_decl_node
 *  The struct declaration visitor pushes its scope as the current scope in the program and iterates over its members
 *
 * Parameters:
 *  <struct ZenitContext> *ctx: Context object
 *  <struct ZenitStructDeclNode> *struct_node: Node to visit
 *  <struct ZenitType> *ctx_type: Contextual type information
 *  <enum InferenceKind> *infer_kind: Contextual information about the inference process
 *
 * Returns:
 *  <struct ZenitSymbol>* - The struct delcaration does not have an associated symbol, so this function returns <NULL>
 */
static struct ZenitSymbol* visit_struct_decl_node(struct ZenitContext *ctx, struct ZenitStructDeclNode *struct_node, struct ZenitType **ctx_type, enum InferenceKind infer_kind)
{
    // Visit the attributes and its properties
    visit_attribute_node_map(ctx, struct_node->attributes);

    zenit_program_push_scope(ctx->program, ZENIT_SCOPE_STRUCT, struct_node->name);

    for (size_t i=0; i < fl_array_length(struct_node->members); i++)
        visit_node(ctx, struct_node->members[i], NULL, INFER_NONE);

    zenit_program_pop_scope(ctx->program);

    return NULL;
}

/*
 * Function: visit_variable_node
 *  The variable visitor share type information with its right-hand side expression it it contains a type hint, or possibly
 *  receives type information from the rhs when the variable does not declare a type.
 *  Also, in case the types are not equals, it tries to add an implicit cast between the rhs type and the lhs type.
 *
 * Parameters:
 *  <struct ZenitContext> *ctx: Context object
 *  <struct ZenitVariableNode> *variable_node: Node to visit
 *  <struct ZenitType> *ctx_type: Contextual type information
 *  <enum InferenceKind> *infer_kind: Contextual information about the inference process
 *
 * Returns:
 *  <struct ZenitSymbol>* - The variable symbol with its -possibly inferred- type information
 */
static struct ZenitSymbol* visit_variable_node(struct ZenitContext *ctx, struct ZenitVariableNode *variable_node, struct ZenitType **ctx_type, enum InferenceKind infer_kind)
{
    // The variable declaration node does not perform type inference in this function, but it phases type information
    // to the right-hand side expression's visitor with possibly INFER_BIDIRECTIONAL when the type is not present in the
    // declaration:
    //  e.g.: var a = [ 1, 2, 3 ]
    //  The INFER_BIDIRECTIONAL is passed to the array's visitor, which means that the type from the array literal ([3]uint8)
    //  will be assigned to the variable's type
    flm_assert(ctx_type == NULL && infer_kind == INFER_NONE, "A variable declaration does not need contextual type information");

    // Visit the attributes and its properties
    visit_attribute_node_map(ctx, variable_node->attributes);

    // We need the symbol we introduced in the <zenit_resolve_symbols> pass
    struct ZenitSymbol *symbol = zenit_program_get_symbol(ctx->program, variable_node->name);

    // We need to get the symbol of the right-hand side expression. (if the variable definition has a type hint, we pass that hint to the visitor)
    struct ZenitSymbol *rhs_symbol = visit_node(ctx, 
                                                variable_node->rvalue, 
                                                &symbol->type, 
                                                // If the variable has a declared type, we don't allow inference on the variable type
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
 *  <struct ZenitContext> *ctx: Context object
 *  <struct ZenitNode> *node: Node to visit
 *  <struct ZenitType> *ctx_type: Contextual type information
 *  <enum InferenceKind> *infer_kind: Contextual information about the inference process
 *
 * Returns:
 *  <struct ZenitSymbol>* - The symbol with its -possibly inferred- type information
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
