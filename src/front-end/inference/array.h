#ifndef ZENIT_INFER_ARRAY_H
#define ZENIT_INFER_ARRAY_H

#include "infer.h"
#include "../phases/utils.h"

/*
 * Function: zenit_infer_types_in_array_node
 *  The array visitor infers the type of the array from its members if the context does not provide type information, or uses the type information
 *  it receives to come up with the array type.
 *
 * Parameters:
 *  <ZenitContext> *ctx: Context object
 *  <ZenitArrayNode> *array_node: Node to visit
 *  <ZenitType> *ctx_type: Contextual type information
 *  <ZenitInferenceKind> *infer_kind: Contextual information about the inference process
 *
 * Returns:
 *  <ZenitSymbol>* - The array symbol with its type information
 */
static inline ZenitSymbol* zenit_infer_types_in_array_node(ZenitContext *ctx, ZenitArrayNode *array_node, ZenitType **ctx_type, ZenitInferenceKind infer_kind)
{
    ZenitSymbol *array_symbol = zenit_utils_get_tmp_symbol(ctx->program, (ZenitNode*) array_node);

    // If we receive a type hint from the calling context, we can use it to update our array's member type,
    // and we can also use it for the array elements
    ZenitInferenceKind member_infer_kind = ZENIT_INFER_NONE;
    if (ctx_type != NULL && infer_kind != ZENIT_INFER_NONE)
    {
        bool ctx_type_is_array = (*ctx_type)->typekind == ZENIT_TYPE_ARRAY;

        if (ctx_type_is_array)
        {            
            if (zenit_type_can_unify(array_symbol->type, *ctx_type))
            {
                zenit_try_type_unification(ctx->types, zenit_infer_ik_to_uk(infer_kind), &array_symbol->type, ctx_type);
            }
            else
            {
                // If the array types cannot be unified, it could be because of a different length between the arrays, so 
                // let's try directly with their member types
                ZenitArrayType *array_type = (ZenitArrayType*) array_symbol->type;
                ZenitArrayType *ctx_array_type = (ZenitArrayType*) *ctx_type;

                if (zenit_type_can_unify(array_type->member_type, ctx_array_type->member_type))
                    zenit_try_type_unification(ctx->types, zenit_infer_ik_to_uk(infer_kind), &array_type->member_type, &ctx_array_type->member_type);
            }

            // We directly assign the infer_kind of the context to the member_infer_kind because:
            //  - If the context sent ZENIT_INFER_UNIDIRECTIONAL it means there is a type hint out there in the context, and we should honor that
            //  - If the context sent ZENIT_INFER_BIDIRECTIONAL, we want the member visitors to be able to infer the array type and also the context type
            //    if possible
            member_infer_kind = infer_kind;
        }
        else
        {
            // If the hint type is not an array, we don't have much more information, we just allow bidirectional inference
            // and hope for the best (?)
            member_infer_kind = ZENIT_INFER_BIDIRECTIONAL;
        }
    }    

    ZenitArrayType *array_type = (ZenitArrayType*) array_symbol->type;

    ZenitSymbol **elements = fl_array_new(sizeof(ZenitSymbol*), fl_array_length(array_node->elements));

    // Visit each element node and try to unify each element type with the array's member type
    for (size_t i=0; i < fl_array_length(array_node->elements); i++)
    {
        ZenitSymbol *elem_symbol = zenit_infer_types_in_node(ctx, 
                                                array_node->elements[i], 
                                                // We directly pass the array member type
                                                &((ZenitArrayType*) array_symbol->type)->member_type, 
                                                member_infer_kind);

        elements[i] = elem_symbol;
    }

    for (size_t i=0; i < fl_array_length(elements); i++)
    {
        // If the types are not equals, we try to cast the RHS to the LHS type, and if that is not possible, we don't do anything here, we just let
        // the type check to fail later.
        if (!zenit_type_equals(array_type->member_type, elements[i]->type) && zenit_type_is_castable_to(elements[i]->type, array_type->member_type))
        {   
            // NOTE: the ZenitVariableNode structure changes, but we don't need to worry about its UID changing because of that
            // as the variables are always accessed by its name, and not by the UID
            ZenitCastNode *cast_node = zenit_cast_node_new(array_node->elements[i]->location, array_node->elements[i], true);
            array_node->elements[i] = (ZenitNode*) cast_node;

            zenit_utils_new_tmp_symbol(ctx->program, (ZenitNode*) cast_node, zenit_type_ctx_copy_type(ctx->types, array_type->member_type));
        }
    }

    fl_array_free(elements);

    // If the context is waiting for type information (BIDIRECTIONAL) we need to check the array's member type, because we could
    // have updated it when we visited the array elements, and that type information must be shared with the caller:
    //  e.g.:
    //      var a = [ 1, 2, 0x1FF ];
    //
    //  Here the variable declaration pass ZENIT_INFER_BIDIRECTIONAL, and the last element in the array promotes the array member type to
    //  uint16. At this point in the execution, the type of the array is [3]uint16 and we should fill that information in the ctx_type
    //  pointer which is the variable's type information
    zenit_try_type_unification(ctx->types, zenit_infer_ik_to_uk(infer_kind), &array_symbol->type, ctx_type);

    return array_symbol;
}


#endif /* ZENIT_INFER_ARRAY_H */
