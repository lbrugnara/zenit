#include "variable.h"
#include "attribute-map.h"
#include "../phases/utils.h"

/*
 * Function: zenit_infer_types_in_variable_node
 *  The variable visitor share type information with its right-hand side expression if it contains a type hint, or possibly
 *  receives type information from the rhs when the variable does not declare a type.
 *  Also, in case the types are not equals, it tries to add an implicit cast between the rhs type and the lhs type.
 *
 * Parameters:
 *  <ZenitContext> *ctx: Context object
 *  <ZenitVariableNode> *variable_node: Node to visit
 *  <ZenitType> *ctx_type: Contextual type information
 *  <ZenitInferenceKind> *infer_kind: Contextual information about the inference process
 *
 * Returns:
 *  <ZenitSymbol>* - The variable symbol with its -possibly inferred- type information
 */
ZenitSymbol* zenit_infer_types_in_variable_node(ZenitContext *ctx, ZenitVariableNode *variable_node, ZenitType **ctx_type, ZenitInferenceKind infer_kind)
{
    // The variable declaration node does not perform type inference in this function, but it passes type information
    // to the right-hand side expression's visitor with possibly ZENIT_INFER_BIDIRECTIONAL when the type is not present in the
    // declaration:
    //  e.g.: var a = [ 1, 2, 3 ]
    //  The ZENIT_INFER_BIDIRECTIONAL is passed to the array's visitor, which means that the type from the array literal ([3]uint8)
    //  will be assigned to the variable's type
    zenit_assert(ctx_type == NULL && infer_kind == ZENIT_INFER_NONE, "A variable declaration does not need contextual type information");

    // Visit the attributes and its properties
    zenit_infer_types_in_attribute_node_map(ctx, variable_node->attributes);

    // We need the symbol we introduced in the <zenit_resolve_symbols> pass
    ZenitSymbol *symbol = zenit_program_get_symbol(ctx->program, variable_node->name);

    // We need to get the symbol of the right-hand side expression. (if the variable definition has a type hint, we pass that hint to the visitor)
    ZenitSymbol *rhs_symbol = zenit_infer_types_in_node(ctx, 
                                            variable_node->rvalue, 
                                            &symbol->type, 
                                            // If the variable has a declared type, we don't allow inference on the variable type
                                            variable_node->type_decl != NULL
                                                ? ZENIT_INFER_UNIDIRECTIONAL 
                                                : ZENIT_INFER_BIDIRECTIONAL);

    // If the types are not equals, we try to cast the RHS to the LHS type, and if that is not possible, we don't do anything here, we just let
    // the type check to fail later.
    if (!zenit_type_equals(symbol->type, rhs_symbol->type) && zenit_type_is_castable_to(rhs_symbol->type, symbol->type))
    {   
        // NOTE: the ZenitVariableNode structure changes, but we don't need to worry about its UID changing because
        // the variables are always accessed by its name, and not by the UID
        ZenitCastNode *cast_node = zenit_cast_node_new(variable_node->rvalue->location, variable_node->rvalue, true);
        variable_node->rvalue = (ZenitNode*) cast_node;

        zenit_utils_new_tmp_symbol(ctx->program, (ZenitNode*) cast_node, zenit_type_ctx_copy_type(ctx->types, symbol->type));
    }

    // We always return the variable symbol
    return symbol;
}
