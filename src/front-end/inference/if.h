#ifndef ZENIT_INFER_IF_H
#define ZENIT_INFER_IF_H

#include "infer.h"
#include "../phases/utils.h"

static inline ZenitSymbol* zenit_infer_types_in_if_node(ZenitContext *ctx, ZenitIfNode *if_node, ZenitType **ctx_type, ZenitInferenceKind infer_kind)
{
    // Enter to the if's scope
    zenit_program_push_scope(ctx->program, ZENIT_SCOPE_BLOCK, if_node->id);

    // We create a temporary bool type for the condition (no worries about freeing its memory,
    // the types pool will do it later)
    ZenitType *bool_type = (ZenitType*) zenit_type_ctx_new_bool(ctx->types);

    // Evaluate the condition expression
    ZenitSymbol *condition_symbol = zenit_infer_types_in_node(ctx, if_node->condition, &bool_type, ZENIT_INFER_UNIDIRECTIONAL);

    // If the types are not equals, we try to cast the condition to a boolean, and if that is not possible, we don't do anything 
    // here, we just let the type check to fail later.
    if (!zenit_type_equals(bool_type, condition_symbol->type) && zenit_type_is_castable_to(condition_symbol->type, bool_type))
    {   
        // NOTE: the ZenitVariableNode structure changes, but we don't need to worry about its UID changing because
        // the variables are always accessed by its name, and not by the UID
        ZenitCastNode *cast_node = zenit_cast_node_new(if_node->condition->location, if_node->condition, true);
        if_node->condition = (ZenitNode*) cast_node;

        zenit_utils_new_tmp_symbol(ctx->program, (ZenitNode*) cast_node, bool_type);
    }

    // Evaluate the then branch (no need to infer anything)
    zenit_infer_types_in_node(ctx, if_node->then_branch, NULL, ZENIT_INFER_NONE);

    // If present, evaluate the then branch (no need to infer anything)
    if (if_node->else_branch)
        zenit_infer_types_in_node(ctx, if_node->else_branch, NULL, ZENIT_INFER_NONE);

    // Move out of the if scope
    zenit_program_pop_scope(ctx->program);

    return NULL;
}

#endif /* ZENIT_INFER_IF_H */
