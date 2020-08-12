#ifndef ZENIT_INFER_IF_H
#define ZENIT_INFER_IF_H

#include "infer.h"

ZenitSymbol* zenit_infer_types_in_if_node(ZenitContext *ctx, ZenitIfNode *if_node, ZenitType **ctx_type, ZenitInferenceKind infer_kind);

#endif /* ZENIT_INFER_IF_H */
