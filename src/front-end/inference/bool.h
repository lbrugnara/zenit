#ifndef ZENIT_INFER_BOOL_H
#define ZENIT_INFER_BOOL_H

#include "infer.h"

ZenitSymbol* zenit_infer_types_in_bool_node(ZenitContext *ctx, ZenitBoolNode *bool_node, ZenitType **ctx_type, ZenitInferenceKind infer_kind);

#endif /* ZENIT_INFER_BOOL_H */
