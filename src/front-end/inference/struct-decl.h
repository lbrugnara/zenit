#ifndef ZENIT_INFER_STRUCT_DECL_H
#define ZENIT_INFER_STRUCT_DECL_H

#include "infer.h"

ZenitSymbol* zenit_infer_types_in_struct_decl_node(ZenitContext *ctx, ZenitStructDeclNode *struct_node, ZenitType **ctx_type, ZenitInferenceKind infer_kind);

#endif /* ZENIT_INFER_STRUCT_DECL_H */
