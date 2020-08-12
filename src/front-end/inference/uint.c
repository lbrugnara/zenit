#include "uint.h"
#include "../phases/utils.h"

/*
 * Function: zenit_infer_types_in_uint_node
 *  A uint node always has a type therefore we just return it
 *
 * Parameters:
 *  <ZenitContext> *ctx - Context object
 *  <ZenitUintNode> *uint_node - Uint node
 *  <ZenitType> *ctx_type: Contextual type information
 *  <ZenitInferenceKind> *infer_kind: Contextual information about the inference process
 *
 * Returns:
 *  <ZenitSymbol>*: The temporal symbol of the uint literal
 */
ZenitSymbol* zenit_infer_types_in_uint_node(ZenitContext *ctx, ZenitUintNode *uint_node, ZenitType **ctx_type, ZenitInferenceKind infer_kind)
{
    ZenitSymbol *uint_symbol = zenit_utils_get_tmp_symbol(ctx->program, (ZenitNode*) uint_node);

    // NOTE: The uint type is intrinsic, it can't be updated/modified, but the context might ask for type 
    // information, and in that case, we update just the ctx_type object
    if (ctx_type != NULL && infer_kind == ZENIT_INFER_BIDIRECTIONAL)
        zenit_try_type_unification(ctx->types, ZENIT_UNIFY_B, &uint_symbol->type, ctx_type);

    return uint_symbol;
}
