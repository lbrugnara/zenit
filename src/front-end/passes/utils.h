#ifndef ZENIT_UTILS_H
#define ZENIT_UTILS_H

#include "../program.h"
#include "../symbol.h"
#include "../context.h"
#include "../ast/ast.h"
#include "../types/system.h"

static struct ZenitTypeInfo* build_type_info_from_declaration(struct ZenitContext *ctx, struct ZenitTypeNode *type_decl, struct ZenitTypeInfo *rhs)
{
    struct ZenitTypeInfo *typeinfo = NULL;

    if (type_decl == NULL)
    {
        typeinfo = zenit_typeinfo_new(ZENIT_TYPE_SRC_INFERRED, false, zenit_type_none_new());
    }
    else if (type_decl->base.type == ZENIT_NODE_TYPE_UINT)
    {
        struct ZenitUintTypeNode *uint_type_decl = (struct ZenitUintTypeNode*) type_decl;
        typeinfo = zenit_typeinfo_new_uint(ZENIT_TYPE_SRC_HINT, true, zenit_type_uint_new(uint_type_decl->size));
    }
    else if (type_decl->base.type == ZENIT_NODE_TYPE_STRUCT)
    {
        struct ZenitStructTypeNode *struct_type_decl = (struct ZenitStructTypeNode*) type_decl;
        typeinfo = zenit_typeinfo_new_struct(ZENIT_TYPE_SRC_HINT, true, zenit_type_struct_new(struct_type_decl->name));
    }
    else if (type_decl->base.type == ZENIT_NODE_TYPE_REFERENCE)
    {
        struct ZenitReferenceTypeNode *ref_type_decl = (struct ZenitReferenceTypeNode*) type_decl;

        struct ZenitTypeInfo *rhs_element = NULL;
        if (rhs != NULL && rhs->type->typekind == ZENIT_TYPE_REFERENCE)
            rhs_element = ((struct ZenitReferenceType*) rhs->type)->element;

        struct ZenitTypeInfo *element_typeinfo = build_type_info_from_declaration(ctx, ref_type_decl->element, rhs_element);
        typeinfo = zenit_typeinfo_new_reference(ZENIT_TYPE_SRC_HINT, true, zenit_type_reference_new(element_typeinfo));
    }
    else if (type_decl->base.type == ZENIT_NODE_TYPE_ARRAY)
    {
        struct ZenitArrayTypeNode *array_type_decl = (struct ZenitArrayTypeNode*) type_decl;

        size_t length = 0;
        struct ZenitTypeInfo *rhs_element = NULL;
        if (rhs != NULL && rhs->type->typekind == ZENIT_TYPE_ARRAY)
        {
            rhs_element = ((struct ZenitArrayType*) rhs->type)->member_type;
            length = ((struct ZenitArrayType*) rhs->type)->length;
        }
        else if (array_type_decl->auto_length)
        {
            zenit_context_error(ctx, array_type_decl->base.base.location, ZENIT_ERROR_INFERENCE, 
                "Cannot infer the length of the array type from the context. Try adding the array length a hint into the variable's type information.");
        }

        struct ZenitArrayType *array_type = zenit_type_array_new(build_type_info_from_declaration(ctx, array_type_decl->member_type, rhs_element));
        array_type->length = array_type_decl->auto_length ? length : array_type_decl->length;

        typeinfo = zenit_typeinfo_new_array(ZENIT_TYPE_SRC_HINT, true, array_type);
    }
    else
    {
        typeinfo = zenit_typeinfo_new(ZENIT_TYPE_SRC_INFERRED, false, zenit_type_none_new());
    }

    // We track the type with the program object because the program will take care of freeing the memory
    // once it is freed too
    return typeinfo;
}

static inline struct ZenitSymbol* zenit_utils_new_tmp_symbol(struct ZenitProgram *program, struct ZenitNode *node, struct ZenitTypeInfo *typeinfo)
{
    char *name = zenit_node_uid(node);
    if (zenit_program_has_symbol(program, name))
    {
        fl_cstring_free(name);
        return NULL;
    }

    struct ZenitSymbol *symbol = zenit_symbol_new(name, typeinfo);
    
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
