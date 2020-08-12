#ifndef ZENIT_INFER_FIELD_DECL_H
#define ZENIT_INFER_FIELD_DECL_H

#include "infer.h"

ZenitSymbol* zenit_infer_types_in_field_decl_node(ZenitContext *ctx, ZenitFieldDeclNode *field_node, ZenitType **ctx_type, ZenitInferenceKind infer_kind);

#endif /* ZENIT_INFER_FIELD_DECL_H */
