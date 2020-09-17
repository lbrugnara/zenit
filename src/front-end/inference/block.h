#ifndef ZENIT_INFER_BLOCK_H
#define ZENIT_INFER_BLOCK_H

#include "infer.h"
#include "../utils.h"

static inline ZenitSymbol* zenit_infer_types_in_block_node(ZenitContext *ctx, ZenitBlockNode *block_node, ZenitType **ctx_type, ZenitInferenceKind infer_kind)
{
    zenit_program_push_scope(ctx->program, ZENIT_SCOPE_BLOCK, block_node->id);

    for (size_t i = 0; i < fl_array_length(block_node->statements); i++)
        zenit_infer_types_in_node(ctx, block_node->statements[i], NULL, ZENIT_INFER_NONE);

    zenit_program_pop_scope(ctx->program);

    return NULL;
}

#endif /* ZENIT_INFER_BLOCK_H */
