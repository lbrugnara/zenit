#ifndef ZENIT_INFER_REFERENCE_H
#define ZENIT_INFER_REFERENCE_H

#include "infer.h"
#include "../utils.h"

/*
 * Function: zenit_infer_types_in_reference_node
 *  The reference visitor just visits (pun intended) the referenced expression. The type information of that expression is already present
 *  in the reference type (see the resolve pass), so we don't need to "unify" its types. What we can do is, if the context asks for type information
 *  (ZENIT_INFER_BIDIRECTIONAL) populates the *ctx_type* object with the reference type.
 *
 * Parameters:
 *  <ZenitContext> *ctx: Context object
 *  <ZenitReferenceNode> *reference_node: Node to visit
 *  <ZenitType> *ctx_type: Contextual type information
 *  <ZenitInferenceKind> *infer_kind: Contextual information about the inference process
 *
 * Returns:
 *  <ZenitSymbol>* - The reference symbol with its type information
 */
static inline ZenitSymbol* zenit_infer_types_in_reference_node(ZenitContext *ctx, ZenitReferenceNode *reference_node, ZenitType **ctx_type, ZenitInferenceKind infer_kind)
{
    ZenitSymbol *ref_symbol = zenit_utils_get_tmp_symbol(ctx->program, (ZenitNode*) reference_node);
    ZenitReferenceType *ref_type = (ZenitReferenceType*) ref_symbol->type;

    // The referenced element type is inferred (check the resolve pass) so it is ok to ask for bidirectional inference here
    zenit_infer_types_in_node(ctx, reference_node->expression, &ref_type->element, ZENIT_INFER_BIDIRECTIONAL);

    // Now it's time to check if we should update the reference type or update the contextual type
    zenit_try_type_unification(ctx->types, zenit_infer_ik_to_uk(infer_kind), &ref_symbol->type, ctx_type);

    return ref_symbol;
}

#endif /* ZENIT_INFER_REFERENCE_H */
