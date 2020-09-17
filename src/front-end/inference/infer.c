#include "infer.h"

#include "array.h"
#include "block.h"
#include "bool.h"
#include "cast.h"
#include "field-decl.h"
#include "identifier.h"
#include "if.h"
#include "reference.h"
#include "struct.h"
#include "struct-decl.h"
#include "uint.h"
#include "variable.h"

#include "../phases/utils.h"
#include "../program.h"
#include "../symbol.h"
#include "../types/context.h"

/*
 * Variable: inferrers
 *  An array indexed with a <ZenitNodeKind> to get a <ZenitTypeInferrer> function
 */
static const ZenitTypeInferrer inferrers[] = {
    [ZENIT_AST_NODE_UINT]           = (ZenitTypeInferrer) &zenit_infer_types_in_uint_node,
    [ZENIT_AST_NODE_BOOL]           = (ZenitTypeInferrer) &zenit_infer_types_in_bool_node,
    [ZENIT_AST_NODE_VARIABLE]       = (ZenitTypeInferrer) &zenit_infer_types_in_variable_node,
    [ZENIT_AST_NODE_ARRAY]          = (ZenitTypeInferrer) &zenit_infer_types_in_array_node,
    [ZENIT_AST_NODE_IDENTIFIER]     = (ZenitTypeInferrer) &zenit_infer_types_in_identifier_node,
    [ZENIT_AST_NODE_REFERENCE]      = (ZenitTypeInferrer) &zenit_infer_types_in_reference_node,
    [ZENIT_AST_NODE_CAST]           = (ZenitTypeInferrer) &zenit_infer_types_in_cast_node,
    [ZENIT_AST_NODE_FIELD_DECL]     = (ZenitTypeInferrer) &zenit_infer_types_in_field_decl_node,
    [ZENIT_AST_NODE_STRUCT_DECL]    = (ZenitTypeInferrer) &zenit_infer_types_in_struct_decl_node,
    [ZENIT_AST_NODE_STRUCT]         = (ZenitTypeInferrer) &zenit_infer_types_in_struct_node,
    [ZENIT_AST_NODE_IF]             = (ZenitTypeInferrer) &zenit_infer_types_in_if_node,
    [ZENIT_AST_NODE_BLOCK]          = (ZenitTypeInferrer) &zenit_infer_types_in_block_node,
};

ZenitUnificationKind zenit_infer_ik_to_uk(ZenitInferenceKind infer_kind)
{
    switch (infer_kind)
    {
        case ZENIT_INFER_NONE:
            return ZENIT_UNIFY_NONE;

        case ZENIT_INFER_UNIDIRECTIONAL:
            return ZENIT_UNIFY_A;

        case ZENIT_INFER_BIDIRECTIONAL:
            return ZENIT_UNIFY_ALL;
    }

    return ZENIT_UNIFY_NONE;
}

/*
 * Function: zenit_try_type_unification
 *  This function receives two pointers to type objects and tries to find the common ancestor between the two types, and 
 *  in case it exists, this function based on the value of the *unification_kind* variable updates each type accordingly.
 *
 * Parameters:
 *  <ZenitTypeContext> *type_ctx: Type context object
 *  <ZenitUnificationKind> unification_kind: Tells the function which type it needs to update
 *  <ZenitType> **type_a: Pointer to a type object. If *unification_kind* is ZENIT_UNIFY_A or ZENIT_UNIFY_ALL, the function updates the type if there
 *                               is a common ancestor between the types
 *  <ZenitType> **type_b: Pointer to a type object. If *unification_kind* is ZENIT_UNIFY_B or ZENIT_UNIFY_ALL, the function updates the type if there
 *                               is a common ancestor between the types
 *
 * Returns:
 *  void: This function does not return a value
 */
void zenit_try_type_unification(ZenitTypeContext *type_ctx, ZenitUnificationKind unification_kind, ZenitType **type_a, ZenitType **type_b)
{
    if (unification_kind == ZENIT_UNIFY_NONE)
        return;

    if (zenit_type_can_unify(*type_a, *type_b) && !zenit_type_equals(*type_a, *type_b))
    {
        ZenitType *unified_type = NULL;
        if (zenit_type_ctx_unify_types(type_ctx, *type_a, *type_b, &unified_type))
        {
            if ((unification_kind == ZENIT_UNIFY_A || unification_kind == ZENIT_UNIFY_ALL) && !zenit_type_equals(*type_a, unified_type))
                *type_a = zenit_type_ctx_copy_type(type_ctx, unified_type);

            if ((unification_kind == ZENIT_UNIFY_B || unification_kind == ZENIT_UNIFY_ALL) && !zenit_type_equals(*type_b, unified_type))
                *type_b = zenit_type_ctx_copy_type(type_ctx, unified_type);
        }
    }
}

/*
 * Function: visit_node
 *  This function selects the visitor function based on the node's type
 *  and calls the function.
 *
 * Parameters:
 *  <ZenitContext> *ctx: Context object
 *  <ZenitNode> *node: Node to visit
 *  <ZenitType> *ctx_type: Contextual type information
 *  <ZenitInferenceKind> *infer_kind: Contextual information about the inference process
 *
 * Returns:
 *  <ZenitSymbol>* - The symbol with its -possibly inferred- type information
 */
ZenitSymbol* zenit_infer_types_in_node(ZenitContext *ctx, ZenitNode *node, ZenitType **ctx_type, ZenitInferenceKind infer_kind)
{
    return inferrers[node->nodekind](ctx, node, ctx_type, infer_kind);
}

/*
 * Function: zenit_infer_symbols
 *  We just iterate over the declarations visiting each node
 */
bool zenit_infer_types(ZenitContext *ctx)
{
    if (!ctx || !ctx->ast || !ctx->ast->decls)
        return false;

    size_t errors = zenit_context_error_count(ctx);

    for (size_t i=0; i < fl_array_length(ctx->ast->decls); i++)
        zenit_infer_types_in_node(ctx, ctx->ast->decls[i], NULL, ZENIT_INFER_NONE);

    return errors == zenit_context_error_count(ctx);
}
