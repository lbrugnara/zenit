#ifndef ZENIT_UTILS_H
#define ZENIT_UTILS_H

#include "../program.h"
#include "../symbol.h"
#include "../context.h"
#include "../ast/ast.h"
#include "../types/context.h"

#ifdef DEBUG
     /*
     * Macro: zenit_assert
     * ===== C =====
     *  #define zenit_assert(condition, error) ((condition) ? (flm_exit(ERR_FATAL, "Condition '" #condition "' failed. " error), 0) : 1)
     * =============
     *  Checks the *condition*, and if it is false, it calls the <fl_exit> function with the
     *  message error defined in *error*
     *
     * Parameters:
     *  bool condition - Expression that returns a boolean value
     *  const char *error - Error message
     * 
     * Notes:
     *  - If the condition evaluates to false, the macro calls <fl_exit> therefore in that situation it does not return
     *  - This macro evaluates only when the <FL_DEBUG> flag evaluates to 1
     *
     */
    #define zenit_assert(condition, error) (!(condition) ? (flm_exit(ERR_FATAL, "Condition '" #condition "' failed. " error), 0) : 1)

     /*
      * Macro: zenit_vassert
      * ===== C =====
      *  #define zenit_vassert(condition, error, ...) (!(condition) ? (flm_vexit(ERR_FATAL, "Condition '" #condition "' failed. " error, __VA_ARGS__), 0) : 1)
      * =============
      *  Checks the *condition*, and if it is false, it calls the <fl_exit> function with the
      *  message error defined in *error* formatted with the additional arguments.
      *
      * Parameters:
      *     bool condition - Expression that returns a boolean value
      *     const char *error - Error message that supports format specifiers
      *     ... arguments - Additional arguments used to replace format specifiers in *error*
      *
      * Notes:
      *  - If the condition evaluates to false, the macro calls <fl_exit> therefore in that situation it does not return
      *  - This macro evaluates only when the <FL_DEBUG> flag evaluates to 1
      */
    #define zenit_vassert(condition, error, ...) (!(condition) ? (flm_vexit(ERR_FATAL, "Condition '" #condition "' failed. " error, __VA_ARGS__), 0) : 1)
#else
    /* Mute the assertions */
    #define zenit_assert(condition, error) 1
    #define zenit_vassert(condition, error, ...) 1
#endif

static ZenitType* get_type_from_type_declaration(ZenitContext *ctx, ZenitTypeNode *type_decl, ZenitType *rhs_type)
{
    ZenitType *type = NULL;

    if (type_decl == NULL)
    {
        type = zenit_type_ctx_new_none(ctx->types);
    }
    else if (type_decl->base.nodekind == ZENIT_NODE_TYPE_UINT)
    {
        ZenitUintTypeNode *uint_type_decl = (ZenitUintTypeNode*) type_decl;
        type = (ZenitType*) zenit_type_ctx_new_uint(ctx->types, uint_type_decl->size);
    }
    else if (type_decl->base.nodekind == ZENIT_NODE_TYPE_BOOL)
    {
        ZenitBoolTypeNode *bool_type_decl = (ZenitBoolTypeNode*) type_decl;
        type = (ZenitType*) zenit_type_ctx_new_bool(ctx->types);
    }
    else if (type_decl->base.nodekind == ZENIT_NODE_TYPE_STRUCT)
    {
        ZenitStructTypeNode *struct_type_decl = (ZenitStructTypeNode*) type_decl;
        type = (ZenitType*) zenit_type_ctx_new_struct(ctx->types, struct_type_decl->name);
    }
    else if (type_decl->base.nodekind == ZENIT_NODE_TYPE_REFERENCE)
    {
        ZenitReferenceTypeNode *ref_type_decl = (ZenitReferenceTypeNode*) type_decl;

        ZenitType *rhs_element_type = NULL;
        if (rhs_type != NULL && rhs_type->typekind == ZENIT_TYPE_REFERENCE)
        {
            rhs_element_type = ((ZenitReferenceType*) rhs_type)->element;
        }

        ZenitType *element_type = get_type_from_type_declaration(ctx, ref_type_decl->element, rhs_element_type != NULL ? rhs_element_type : NULL);
        type = (ZenitType*) zenit_type_ctx_new_reference(ctx->types, element_type);
    }
    else if (type_decl->base.nodekind == ZENIT_NODE_TYPE_ARRAY)
    {
        ZenitArrayTypeNode *array_type_decl = (ZenitArrayTypeNode*) type_decl;

        size_t length = 0;
        ZenitType *rhs_element_type = NULL;
        if (rhs_type != NULL && rhs_type->typekind == ZENIT_TYPE_ARRAY)
        {
            rhs_element_type = ((ZenitArrayType*) rhs_type)->member_type;
            length = ((ZenitArrayType*) rhs_type)->length;
        }
        else if (array_type_decl->auto_length)
        {
            zenit_context_error(ctx, array_type_decl->base.base.location, ZENIT_ERROR_INFERENCE, 
                "Cannot infer the length of the array type from the context. Try adding the array length a hint into the variable's type information.");
        }

        ZenitType *member_type = get_type_from_type_declaration(ctx, array_type_decl->member_type, rhs_element_type != NULL ? rhs_element_type : NULL);

        ZenitArrayType *array_type = zenit_type_ctx_new_array(ctx->types, member_type);
        array_type->length = array_type_decl->auto_length ? length : array_type_decl->length;

        type = (ZenitType*) array_type;
    }
    else
    {
        type = zenit_type_ctx_new_none(ctx->types);
    }

    return type;
}

static inline ZenitSymbol* zenit_utils_new_tmp_symbol(ZenitProgram *program, ZenitNode *node, ZenitType *type)
{
    char *name = zenit_node_uid(node);
    if (zenit_program_has_symbol(program, name))
    {
        fl_cstring_free(name);
        return NULL;
    }

    ZenitSymbol *symbol = zenit_symbol_new(name, type);
    
    ZenitSymbol *result = zenit_program_add_symbol(program, symbol);

    if (result == NULL)
        zenit_symbol_free(symbol);

    fl_cstring_free(name);

    return result;
}

static inline ZenitSymbol* zenit_utils_get_update_symbol(ZenitProgram *program, ZenitNode *node, const char *old_name)
{
    if (!zenit_program_has_symbol(program, old_name))
        return NULL;

    ZenitSymbol *symbol = zenit_program_remove_symbol(program, old_name);

    char *name = zenit_node_uid(node);

    // FIXME: We can add an "update" function in the symbol module
    if (symbol->name)
        fl_cstring_free(symbol->name);

    symbol->name = name;

    zenit_program_add_symbol(program, symbol);

    return symbol;
}


static inline ZenitSymbol* zenit_utils_get_tmp_symbol(ZenitProgram *program, ZenitNode *node)
{
    char *name = zenit_node_uid(node);
    ZenitSymbol *symbol = zenit_program_get_symbol(program, name);

    fl_cstring_free(name);

    return symbol;
}

static inline ZenitSymbol* zenit_utils_remove_tmp_symbol(ZenitProgram *program, ZenitNode *node)
{
    char *name = zenit_node_uid(node);
    
    ZenitSymbol *symbol = zenit_program_remove_symbol(program, name);

    fl_cstring_free(name);

    return symbol;
}

#endif /* ZENIT_UTILS_H */
