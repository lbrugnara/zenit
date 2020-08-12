#ifndef ZENIT_INFER_IDENTIFIER_H
#define ZENIT_INFER_IDENTIFIER_H

#include "infer.h"

ZenitSymbol* zenit_infer_types_in_identifier_node(ZenitContext *ctx, ZenitIdentifierNode *node, ZenitType **ctx_type, ZenitInferenceKind infer_kind);

#endif /* ZENIT_INFER_IDENTIFIER_H */
