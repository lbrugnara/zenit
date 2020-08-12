#ifndef ZENIT_INFER_BLOCK_H
#define ZENIT_INFER_BLOCK_H

#include "infer.h"

ZenitSymbol* zenit_infer_types_in_block_node(ZenitContext *ctx, ZenitBlockNode *block_node, ZenitType **ctx_type, ZenitInferenceKind infer_kind);

#endif /* ZENIT_INFER_BLOCK_H */
