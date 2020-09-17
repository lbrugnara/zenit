#ifndef ZENIT_INFER_FIELD_DECL_H
#define ZENIT_INFER_FIELD_DECL_H

#include "infer.h"
#include "../phases/utils.h"

/*
 * Function: zenit_infer_types_in_field_decl_node
 *  The field declaration visitor returns the field symbol, as its type information is always present in the declaration
 *
 * Parameters:
 *  <ZenitContext> *ctx: Context object
 *  <ZenitStructFieldDeclNode> *field_node: Node to visit
 *  <ZenitType> *ctx_type: Contextual type information
 *  <ZenitInferenceKind> *infer_kind: Contextual information about the inference process
 *
 * Returns:
 *  <ZenitSymbol>* - The field symbol with its type information
 */
static inline ZenitSymbol* zenit_infer_types_in_field_decl_node(ZenitContext *ctx, ZenitStructFieldDeclNode *field_node, ZenitType **ctx_type, ZenitInferenceKind infer_kind)
{
    return zenit_program_get_symbol(ctx->program, field_node->name);
}

#endif /* ZENIT_INFER_FIELD_DECL_H */
