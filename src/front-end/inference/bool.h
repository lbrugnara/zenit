#ifndef ZENIT_INFER_BOOL_H
#define ZENIT_INFER_BOOL_H

#include "infer.h"
#include "../utils.h"

/*
 * Function: zenit_infer_types_in_bool_node
 *  A bool node always has a type therefore we just return it
 *
 * Parameters:
 *  <ZenitContext> *ctx - Context object
 *  <ZenitBoolNode> *bool_node - Boolean node
 *  <ZenitType> *ctx_type: Contextual type information
 *  <ZenitInferenceKind> *infer_kind: Contextual information about the inference process
 *
 * Returns:
 *  <ZenitSymbol>*: The temporal symbol of the boolean literal
 */
static inline ZenitSymbol* zenit_infer_types_in_bool_node(ZenitContext *ctx, ZenitBoolNode *bool_node, ZenitType **ctx_type, ZenitInferenceKind infer_kind)
{
    ZenitSymbol *bool_symbol = zenit_utils_get_tmp_symbol(ctx->program, (ZenitNode*) bool_node);

    // NOTE: The bool type is intrinsic, it can't be updated/modified, but the context might ask for type 
    // information, and in that case, we update just the ctx_type object
    if (ctx_type != NULL && infer_kind == ZENIT_INFER_BIDIRECTIONAL)
        zenit_try_type_unification(ctx->types, ZENIT_UNIFY_B, &bool_symbol->type, ctx_type);

    return bool_symbol;
}

#endif /* ZENIT_INFER_BOOL_H */
