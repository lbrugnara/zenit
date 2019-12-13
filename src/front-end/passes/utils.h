#ifndef ZENIT_UTILS_H
#define ZENIT_UTILS_H

#include "../program.h"
#include "../symbol.h"
#include "../ast/ast.h"
#include "../types/system.h"

static struct ZenitTypeInfo* build_type_info_from_declaration(struct ZenitTypeNode *type_decl)
{
    struct ZenitTypeInfo *typeinfo = NULL;

    if (type_decl == NULL)
    {
        typeinfo = zenit_type_none_new();
    }
    else if (type_decl->base.type == ZENIT_NODE_TYPE_UINT)
    {
        struct ZenitUintTypeNode *uint_type_decl = (struct ZenitUintTypeNode*) type_decl;
        typeinfo = (struct ZenitTypeInfo*) zenit_type_uint_new(ZENIT_TYPE_SRC_HINT, uint_type_decl->size);
    }
    else if (type_decl->base.type == ZENIT_NODE_TYPE_STRUCT)
    {
        // FIXME: Add members once they are implemented
        struct ZenitStructTypeNode *struct_type_decl = (struct ZenitStructTypeNode*) type_decl;
        typeinfo = (struct ZenitTypeInfo*) zenit_type_struct_new(ZENIT_TYPE_SRC_HINT, struct_type_decl->name);
    }
    else if (type_decl->base.type == ZENIT_NODE_TYPE_REFERENCE)
    {
        struct ZenitReferenceTypeNode *ref_type_decl = (struct ZenitReferenceTypeNode*) type_decl;
        typeinfo = (struct ZenitTypeInfo*) zenit_type_reference_new(ZENIT_TYPE_SRC_HINT, build_type_info_from_declaration(ref_type_decl->element));
    }
    else if (type_decl->base.type == ZENIT_NODE_TYPE_ARRAY)
    {
        struct ZenitArrayTypeNode *array_type_decl = (struct ZenitArrayTypeNode*) type_decl;
        struct ZenitArrayTypeInfo *array_type = zenit_type_array_new(ZENIT_TYPE_SRC_HINT, build_type_info_from_declaration(array_type_decl->members_type));
        array_type->length = array_type_decl->length;

        // We know the length of the array, and we know the declared type, so we populate the array type with every
        // element and its declared type
        for (size_t i=0; i < array_type->length; i++)
            zenit_type_array_add_member(array_type, build_type_info_from_declaration(array_type_decl->members_type));

        typeinfo = (struct ZenitTypeInfo*) array_type;
        
    }
    else
    {
        typeinfo = zenit_type_none_new();
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

static inline struct ZenitSymbol* zenit_utils_get_tmp_symbol(struct ZenitProgram *program, struct ZenitNode *node)
{
    char *name = zenit_node_uid(node);
    struct ZenitSymbol *symbol = zenit_program_get_symbol(program, name);

    fl_cstring_free(name);

    return symbol;
}

#endif /* ZENIT_UTILS_H */
