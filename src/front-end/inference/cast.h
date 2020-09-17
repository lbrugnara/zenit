#ifndef ZENIT_INFER_CAST_H
#define ZENIT_INFER_CAST_H

#include "infer.h"
#include "../utils.h"

/*
 * Function: zenit_infer_types_in_cast_node
 *  The cast expression does not infer anything, because first it honors the casted expression's type,
 *  and second, the cast's type is explicit "cast(0x1FF : uint8)" or inferred by another node (like a
 *  variable declaration: "var a : uint8 = cast(0x1FF)"  )
 * 
 * Parameters:
 *  <ZenitContext> *ctx - Context object
 *  <ZenitCastNode> *cast_node - Cast node
 *  <ZenitType> *ctx_type: Contextual type information
 *  <ZenitInferenceKind> *infer_kind: Contextual information about the inference process
 *
 * Returns:
 *  <ZenitSymbol>*: The temporal symbol of the cast expression
 */
static inline ZenitSymbol* zenit_infer_types_in_cast_node(ZenitContext *ctx, ZenitCastNode *cast_node, ZenitType **ctx_type, ZenitInferenceKind infer_kind)
{
    ZenitSymbol *cast_symbol = zenit_utils_get_tmp_symbol(ctx->program, (ZenitNode*) cast_node);
    ZenitSymbol *casted_expression = zenit_infer_types_in_node(ctx, cast_node->expression, NULL, ZENIT_INFER_NONE);

    // NOTE: If the type of the cast is NONE, it means the cast does not contain type information. In that case it is mandatory for the contextual
    // type information to be present and valid, because if not, we can't infer the cast type
    bool missing_cast_type = (ctx_type != NULL && cast_symbol->type->typekind == ZENIT_TYPE_NONE && (*ctx_type)->typekind == ZENIT_TYPE_NONE)
                                || (ctx_type == NULL && cast_symbol->type->typekind == ZENIT_TYPE_NONE);

    // NOTE: If the infer_kind is NONE and the cast does not contain type information, we can't update the cast's type and in that
    // case its type will remain NONE at the end of the infer pass, which is something we don't want.
    bool infer_dead_end = (infer_kind == ZENIT_INFER_NONE && cast_symbol->type->typekind == ZENIT_TYPE_NONE) 
                            || (ctx_type != NULL && infer_kind != ZENIT_INFER_BIDIRECTIONAL && (*ctx_type)->typekind == ZENIT_TYPE_NONE);

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
    zenit_try_type_unification(ctx->types, zenit_infer_ik_to_uk(infer_kind), &cast_symbol->type, ctx_type);

    return cast_symbol;
}

#endif /* ZENIT_INFER_CAST_H */
