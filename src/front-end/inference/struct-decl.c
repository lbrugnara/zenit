#include "struct-decl.h"
#include "attribute-map.h"
#include "../phases/utils.h"

/*
 * Function: zenit_infer_types_in_struct_decl_node
 *  The struct declaration visitor pushes its scope as the current scope in the program and iterates over its members
 *
 * Parameters:
 *  <ZenitContext> *ctx: Context object
 *  <ZenitStructDeclNode> *struct_node: Node to visit
 *  <ZenitType> *ctx_type: Contextual type information
 *  <ZenitInferenceKind> *infer_kind: Contextual information about the inference process
 *
 * Returns:
 *  <ZenitSymbol>* - The struct delcaration does not have an associated symbol, so this function returns <NULL>
 */
ZenitSymbol* zenit_infer_types_in_struct_decl_node(ZenitContext *ctx, ZenitStructDeclNode *struct_node, ZenitType **ctx_type, ZenitInferenceKind infer_kind)
{
    // Visit the attributes and its properties
    zenit_infer_types_in_attribute_node_map(ctx, struct_node->attributes);

    zenit_program_push_scope(ctx->program, ZENIT_SCOPE_STRUCT, struct_node->name);

    for (size_t i=0; i < fl_array_length(struct_node->members); i++)
        zenit_infer_types_in_node(ctx, struct_node->members[i], NULL, ZENIT_INFER_NONE);

    zenit_program_pop_scope(ctx->program);

    return NULL;
}
