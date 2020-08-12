#ifndef ZENIT_INFER_VARIABLE_H
#define ZENIT_INFER_VARIABLE_H

#include "infer.h"

ZenitSymbol* zenit_infer_types_in_variable_node(ZenitContext *ctx, ZenitVariableNode *variable_node, ZenitType **ctx_type, ZenitInferenceKind infer_kind);

#endif /* ZENIT_INFER_VARIABLE_H */
