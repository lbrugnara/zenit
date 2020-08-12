#ifndef ZENIT_INFER_CAST_H
#define ZENIT_INFER_CAST_H

#include "infer.h"

ZenitSymbol* zenit_infer_types_in_cast_node(ZenitContext *ctx, ZenitCastNode *cast_node, ZenitType **ctx_type, ZenitInferenceKind infer_kind);

#endif /* ZENIT_INFER_CAST_H */
