#ifndef ZENIT_INFER_ARRAY_H
#define ZENIT_INFER_ARRAY_H

#include "infer.h"

ZenitSymbol* zenit_infer_types_in_array_node(ZenitContext *ctx, ZenitArrayNode *array_node, ZenitType **ctx_type, ZenitInferenceKind infer_kind);

#endif /* ZENIT_INFER_ARRAY_H */
