#ifndef ZENIT_UTILS_H
#define ZENIT_UTILS_H

#include "../program.h"
#include "../symbol.h"
#include "../context.h"
#include "../ast/ast.h"
#include "../types/context.h"

static struct ZenitType* get_type_from_type_declaration(struct ZenitContext *ctx, struct ZenitTypeNode *type_decl, struct ZenitType *rhs_type)
{
    struct ZenitType *type = NULL;

    if (type_decl == NULL)
    {
        type = zenit_type_ctx_new_none(ctx->types);
    }
    else if (type_decl->base.nodekind == ZENIT_NODE_TYPE_UINT)
    {
        struct ZenitUintTypeNode *uint_type_decl = (struct ZenitUintTypeNode*) type_decl;
        type = (struct ZenitType*) zenit_type_ctx_new_uint(ctx->types, uint_type_decl->size);
    }
    else if (type_decl->base.nodekind == ZENIT_NODE_TYPE_STRUCT)
    {
        struct ZenitStructTypeNode *struct_type_decl = (struct ZenitStructTypeNode*) type_decl;
        type = (struct ZenitType*) zenit_type_ctx_new_struct(ctx->types, struct_type_decl->name);
    }
    else if (type_decl->base.nodekind == ZENIT_NODE_TYPE_REFERENCE)
    {
        struct ZenitReferenceTypeNode *ref_type_decl = (struct ZenitReferenceTypeNode*) type_decl;

        struct ZenitType *rhs_element_type = NULL;
        if (rhs_type != NULL && rhs_type->typekind == ZENIT_TYPE_REFERENCE)
        {
            rhs_element_type = ((struct ZenitReferenceType*) rhs_type)->element;
        }

        struct ZenitType *element_type = get_type_from_type_declaration(ctx, ref_type_decl->element, rhs_element_type != NULL ? rhs_element_type : NULL);
        type = (struct ZenitType*) zenit_type_ctx_new_reference(ctx->types, element_type);
    }
    else if (type_decl->base.nodekind == ZENIT_NODE_TYPE_ARRAY)
    {
        struct ZenitArrayTypeNode *array_type_decl = (struct ZenitArrayTypeNode*) type_decl;

        size_t length = 0;
        struct ZenitType *rhs_element_type = NULL;
        if (rhs_type != NULL && rhs_type->typekind == ZENIT_TYPE_ARRAY)
        {
            rhs_element_type = ((struct ZenitArrayType*) rhs_type)->member_type;
            length = ((struct ZenitArrayType*) rhs_type)->length;
        }
        else if (array_type_decl->auto_length)
        {
            zenit_context_error(ctx, array_type_decl->base.base.location, ZENIT_ERROR_INFERENCE, 
                "Cannot infer the length of the array type from the context. Try adding the array length a hint into the variable's type information.");
        }

        struct ZenitType *member_type = get_type_from_type_declaration(ctx, array_type_decl->member_type, rhs_element_type != NULL ? rhs_element_type : NULL);

        struct ZenitArrayType *array_type = zenit_type_ctx_new_array(ctx->types, member_type);
        array_type->length = array_type_decl->auto_length ? length : array_type_decl->length;

        type = (struct ZenitType*) array_type;
    }
    else
    {
        type = zenit_type_ctx_new_none(ctx->types);
    }

    return type;
}

static inline struct ZenitSymbol* zenit_utils_new_tmp_symbol(struct ZenitProgram *program, struct ZenitNode *node, struct ZenitType *type)
{
    char *name = zenit_node_uid(node);
    if (zenit_program_has_symbol(program, name))
    {
        fl_cstring_free(name);
        return NULL;
    }

    struct ZenitSymbol *symbol = zenit_symbol_new(name, type);
    
    struct ZenitSymbol *result = zenit_program_add_symbol(program, symbol);

    if (result == NULL)
        zenit_symbol_free(symbol);

    fl_cstring_free(name);

    return result;
}

static inline struct ZenitSymbol* zenit_utils_get_update_symbol(struct ZenitProgram *program, struct ZenitNode *node, const char *old_name)
{
    if (!zenit_program_has_symbol(program, old_name))
        return NULL;

    struct ZenitSymbol *symbol = zenit_program_remove_symbol(program, old_name);

    char *name = zenit_node_uid(node);

    // FIXME: We can add an "update" function in the symbol module
    if (symbol->name)
        fl_cstring_free(symbol->name);

    symbol->name = name;

    zenit_program_add_symbol(program, symbol);

    return symbol;
}


static inline struct ZenitSymbol* zenit_utils_get_tmp_symbol(struct ZenitProgram *program, struct ZenitNode *node)
{
    char *name = zenit_node_uid(node);
    struct ZenitSymbol *symbol = zenit_program_get_symbol(program, name);

    fl_cstring_free(name);

    return symbol;
}

static inline struct ZenitSymbol* zenit_utils_remove_tmp_symbol(struct ZenitProgram *program, struct ZenitNode *node)
{
    char *name = zenit_node_uid(node);
    
    struct ZenitSymbol *symbol = zenit_program_remove_symbol(program, name);

    fl_cstring_free(name);

    return symbol;
}

#endif /* ZENIT_UTILS_H */
