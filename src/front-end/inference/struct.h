#ifndef ZENIT_INFER_STRUCT_H
#define ZENIT_INFER_STRUCT_H

#include "infer.h"
#include "../utils.h"

/*
 * Function: visit_named_struct_node
 *  The struct literal visitor iterate over the struct fields and tries to provide contextual type information for them. This visitor doesn't
 *  need to infer anything, as all the type information should be present on the struct declaration, but the type of its fields are used to infer 
 *  the type of the expressions that are used to initialize them.
 *
 * Parameters:
 *  <ZenitContext> *ctx: Context object
 *  <ZenitStructNode> *struct_node: Node to visit
 *  <ZenitType> *ctx_type: Contextual type information
 *  <ZenitInferenceKind> *infer_kind: Contextual information about the inference process
 *
 * Returns:
 *  <ZenitSymbol>* - The struct symbol with its type information
 */
static inline ZenitSymbol* visit_named_struct_node(ZenitContext *ctx, ZenitStructNode *struct_node, ZenitType **ctx_type, ZenitInferenceKind infer_kind)
{
    ZenitSymbol *struct_symbol = zenit_utils_get_tmp_symbol(ctx->program, (ZenitNode*) struct_node);

    // We need the named struct's scope to get type information from its members
    ZenitScope *struct_scope = zenit_program_get_scope(ctx->program, ZENIT_SCOPE_STRUCT, struct_node->name);

    for (size_t i=0; i < fl_array_length(struct_node->members); i++)
    {
        if (struct_node->members[i]->nodekind == ZENIT_AST_NODE_FIELD)
        {
            ZenitStructFieldNode *field_node = (ZenitStructFieldNode*) struct_node->members[i];

            // We retrieve the field declaration to pass it ot the field value's visitor
            ZenitSymbol *field_decl_symbol = zenit_scope_get_symbol(struct_scope, field_node->name);

            // Send the declared type information and disallow inference over the field's type
            zenit_infer_types_in_node(ctx, field_node->value, &field_decl_symbol->type, ZENIT_INFER_UNIDIRECTIONAL);
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
 *  <ZenitContext> *ctx: Context object
 *  <ZenitStructNode> *struct_node: Node to visit
 *  <ZenitType> *ctx_type: Contextual type information
 *  <ZenitInferenceKind> *infer_kind: Contextual information about the inference process
 *
 * Returns:
 *  <ZenitSymbol>* - The struct symbol with its type information
 */
static inline ZenitSymbol* visit_unnamed_struct_node(ZenitContext *ctx, ZenitStructNode *struct_node, ZenitType **ctx_type, ZenitInferenceKind infer_kind)
{
    ZenitSymbol *struct_symbol = zenit_utils_get_tmp_symbol(ctx->program, (ZenitNode*) struct_node);

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
        struct_name = ((ZenitStructType*) *ctx_type)->name;
    }

    // If we have a struct name, we must also have a scope
    ZenitScope *struct_scope = NULL;
    
    if (struct_name != NULL)
        struct_scope = zenit_program_get_scope(ctx->program, ZENIT_SCOPE_STRUCT, struct_name);

    for (size_t i=0; i < fl_array_length(struct_node->members); i++)
    {
        if (struct_node->members[i]->nodekind == ZENIT_AST_NODE_FIELD)
        {
            ZenitStructFieldNode *field_node = (ZenitStructFieldNode*) struct_node->members[i];
            ZenitStructTypeMember *field_member = zenit_struct_type_get_member((ZenitStructType*) struct_symbol->type, field_node->name);

            if (struct_name != NULL)
            {
                // If the struct name is available from the context, we use it to update the member type information
                ZenitSymbol *field_decl_symbol = zenit_scope_get_symbol(struct_scope, field_node->name);

                zenit_try_type_unification(ctx->types, ZENIT_UNIFY_A, &field_member->type, &field_decl_symbol->type);
            }

            // For unnamed structs we allow bidirectional inference. We don't care about the type hint,
            // it can help us, but it is not mandatory
            zenit_infer_types_in_node(ctx, field_node->value, &field_member->type, ZENIT_INFER_BIDIRECTIONAL);
        }
    }

    return struct_symbol;
}

/*
 * Function: zenit_infer_types_in_struct_node
 *  This visitor calls the specific visitor for named and unnamed structs, and after that tries to make a unification between the struct type
 *  and the contextual type information if needed.
 *
 * Parameters:
 *  <ZenitContext> *ctx: Context object
 *  <ZenitStructNode> *struct_node: Node to visit
 *  <ZenitType> *ctx_type: Contextual type information
 *  <ZenitInferenceKind> *infer_kind: Contextual information about the inference process
 *
 * Returns:
 *  <ZenitSymbol>* - The struct symbol with its type information
 */
static inline ZenitSymbol* zenit_infer_types_in_struct_node(ZenitContext *ctx, ZenitStructNode *struct_node, ZenitType **ctx_type, ZenitInferenceKind infer_kind)
{
    ZenitSymbol *struct_symbol = NULL;

    if (struct_node->name != NULL)
        struct_symbol = visit_named_struct_node(ctx, struct_node, ctx_type, infer_kind);
    else
        struct_symbol = visit_unnamed_struct_node(ctx, struct_node, ctx_type, infer_kind);

    zenit_try_type_unification(ctx->types, zenit_infer_ik_to_uk(infer_kind), &struct_symbol->type, ctx_type);

    return struct_symbol;
}

#endif /* ZENIT_INFER_STRUCT_H */
