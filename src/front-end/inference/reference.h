#ifndef ZENIT_INFER_REFERENCE_H
#define ZENIT_INFER_REFERENCE_H

#include "infer.h"

ZenitSymbol* zenit_infer_types_in_reference_node(ZenitContext *ctx, ZenitReferenceNode *reference_node, ZenitType **ctx_type, ZenitInferenceKind infer_kind);

#endif /* ZENIT_INFER_REFERENCE_H */
