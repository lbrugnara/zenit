#ifndef ZENIT_INFER_IDENTIFIER_H
#define ZENIT_INFER_IDENTIFIER_H

#include "infer.h"
#include "../utils.h"

/*
 * Function: zenit_infer_types_in_identifier_node
 *  At this point the symbol must be defined, so we need to retrieve it from the symbol table
 *  and return its type information
 *
 * Parameters:
 *  <ZenitContext> *ctx - Context object
 *  <ZenitIdentifierNode> *id_node - Identifier node
 *  <ZenitType> *ctx_type: Contextual type information
 *  <ZenitInferenceKind> *infer_kind: Contextual information about the inference process
 * 
 * Returns:
 *  <ZenitSymbol>*: The symbol of the identifier
 */
static inline ZenitSymbol* zenit_infer_types_in_identifier_node(ZenitContext *ctx, ZenitIdentifierNode *id_node, ZenitType **ctx_type, ZenitInferenceKind infer_kind)
{
    ZenitSymbol *id_symbol = zenit_program_get_symbol(ctx->program, id_node->name);

    // NOTE: We don't infer the identifier type, because we assume the type information is provided/inferred on the
    // symbol declaration, but we do want to provide type information to the context if it request us that information
    if (ctx_type != NULL && infer_kind == ZENIT_INFER_BIDIRECTIONAL)
        zenit_try_type_unification(ctx->types, ZENIT_UNIFY_B, &id_symbol->type, ctx_type);

    return id_symbol;
}

#endif /* ZENIT_INFER_IDENTIFIER_H */
