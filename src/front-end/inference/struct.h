#ifndef ZENIT_INFER_STRUCT_H
#define ZENIT_INFER_STRUCT_H

#include "infer.h"

ZenitSymbol* zenit_infer_types_in_struct_node(ZenitContext *ctx, ZenitStructNode *struct_node, ZenitType **ctx_type, ZenitInferenceKind infer_kind);

#endif /* ZENIT_INFER_STRUCT_H */
