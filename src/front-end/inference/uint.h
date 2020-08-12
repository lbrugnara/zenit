#ifndef ZENIT_INFER_UINT_H
#define ZENIT_INFER_UINT_H

#include "infer.h"

ZenitSymbol* zenit_infer_types_in_uint_node(ZenitContext *ctx, ZenitUintNode *uint_node, ZenitType **ctx_type, ZenitInferenceKind infer_kind);

#endif /* ZENIT_INFER_UINT_H */
