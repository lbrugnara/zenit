#ifndef ZENIT_INFER_H
#define ZENIT_INFER_H

#include "../ast/ast.h"
#include "../context.h"

typedef enum ZenitInferenceKind {
    ZENIT_INFER_NONE,
    ZENIT_INFER_UNIDIRECTIONAL,
    ZENIT_INFER_BIDIRECTIONAL,
} ZenitInferenceKind;

typedef enum ZenitUnificationKind {
    ZENIT_UNIFY_NONE,
    ZENIT_UNIFY_A,
    ZENIT_UNIFY_B,
    ZENIT_UNIFY_ALL
} ZenitUnificationKind;

ZenitUnificationKind zenit_infer_ik_to_uk(ZenitInferenceKind infer_kind);

typedef ZenitSymbol*(*ZenitTypeInferrer)(ZenitContext *ctx, ZenitNode *node, ZenitType **ctx_type, ZenitInferenceKind infer_kind);
typedef ZenitSymbol*(*ZenitTypePromoter)(ZenitContext *ctx, ZenitNode *node, ZenitType *type);

/*
 * Function: zenit_infer_types
 *  This function does the type inference pass to update symbols with missing
 *  or incomplete type information
 *
 * Parameters:
 *  ctx - Context object
 *
 * Returns:
 *  void - *true* on a pass without errors. Otheriwise, this function returns
 *          *false* and the *errors* property in the <ZenitContext> object
 *          should contain the error(s) description
 * 
 * Notes:
 *  This function updates the type information but does not perform any type
 *  checking. At the end of this pass, if there are no errors, we can say 
 *  all the symbols contain type information, therefore there shouldn't 
 *  be symbols with the <ZENIT_TYPE_NONE> value present in the program
 */
bool zenit_infer_types(ZenitContext *ctx);

ZenitSymbol* zenit_infer_types_in_node(ZenitContext *ctx, ZenitNode *node, ZenitType **ctx_type, ZenitInferenceKind infer_kind);

void zenit_try_type_unification(ZenitTypeContext *type_ctx, ZenitUnificationKind unification_kind, ZenitType **type_a, ZenitType **type_b);

#endif /* ZENIT_INFER_H */
