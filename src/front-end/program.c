#include <fllib/Cstring.h>
#include "program.h"
#include "symbol.h"
#include "types/context.h"

ZenitProgram* zenit_program_new()
{
    ZenitProgram *program = fl_malloc(sizeof(ZenitProgram));
    program->global_scope = zenit_scope_new("global", ZENIT_SCOPE_GLOBAL, NULL);
    program->current_scope = program->global_scope;

    return program;
}

void zenit_program_free(ZenitProgram *program)
{
    if (!program)
        return;
        
    zenit_scope_free(program->global_scope);

    fl_free(program);
}

void zenit_program_add_scope(ZenitProgram *program, ZenitScopeType type, const char *name)
{
    ZenitScope *scope = zenit_scope_new(name, type, program->current_scope);
    program->current_scope->children = fl_array_append(program->current_scope->children, &scope);
}

bool zenit_program_enter_scope(ZenitProgram *program, ZenitScope *scope)
{
    if (scope->parent != program->current_scope)
        return false;

    program->current_scope = scope;
    return true;
}

void zenit_program_push_scope(ZenitProgram *program, ZenitScopeType type, const char *name)
{
    // First we search for an existent scope with the provided type and name    
    ZenitScope *scope = zenit_program_get_scope(program, type, name);

    if (scope == NULL)
    {
        // Add a new scope to the program
        scope = zenit_scope_new(name, type, program->current_scope);
        program->current_scope->children = fl_array_append(program->current_scope->children, &scope);
    }    

    program->current_scope = scope;
}

void zenit_program_pop_scope(ZenitProgram *program)
{
    program->current_scope = program->current_scope->parent;
}

bool zenit_program_has_scope(ZenitProgram *program, ZenitScopeType type, const char *name)
{
    // NOTE: For simplicity we use an array, we could replace this with a hashtable later
    ZenitScope *tmp_scope = program->current_scope;
    do
    {
        for (size_t i=0; i < fl_array_length(tmp_scope->children); i++)
        {
            ZenitScope *scope = tmp_scope->children[i];
            if (flm_cstring_equals(scope->id, name) && scope->type == type)
                return true;
        }
    
        if (type == ZENIT_SCOPE_STRUCT)
            tmp_scope = tmp_scope->parent;
        else break;

    } while (tmp_scope);

    return false;
}

ZenitScope* zenit_program_get_scope(ZenitProgram *program, ZenitScopeType type, const char *name)
{
    // NOTE: For simplicity we use an array, we could replace this with a hashtable later
    ZenitScope *tmp_scope = program->current_scope;
    do
    {
        for (size_t i=0; i < fl_array_length(tmp_scope->children); i++)
        {
            ZenitScope *scope = tmp_scope->children[i];
            if (flm_cstring_equals(scope->id, name) && scope->type == type)
                return scope;
        }

        if (type == ZENIT_SCOPE_STRUCT)
            tmp_scope = tmp_scope->parent;
        else break;

    } while (tmp_scope);    

    return NULL;
}

ZenitSymbol* zenit_program_add_symbol(ZenitProgram *program, ZenitSymbol *symbol)
{
    // FIXME: Fix this to add symbols to the global scope
    return zenit_symtable_add(&program->current_scope->symtable, symbol);
}

bool zenit_program_has_symbol(ZenitProgram *program, const char *symbol_name)
{
    if (program->current_scope->type == ZENIT_SCOPE_GLOBAL)
        return zenit_scope_has_symbol(program->current_scope, symbol_name);

    if (program->current_scope->type == ZENIT_SCOPE_FUNCTION)
    {
        return zenit_scope_has_symbol(program->current_scope, symbol_name)
                || zenit_scope_has_symbol(program->global_scope, symbol_name);
    }

    if (program->current_scope->type == ZENIT_SCOPE_STRUCT)
        return zenit_scope_has_symbol(program->current_scope, symbol_name);

    ZenitScope *scope = program->current_scope;
    do {
        if (zenit_scope_has_symbol(scope, symbol_name))
            return true;

        // If we couldn't find the symbol in the scope, try going to the parent scope
        scope = scope->parent;

        if (scope == NULL)
            break;

        // If the parent scope is a function, the only possible place the symbol could be
        // is within the function's scope or the global scope
        if (scope->type == ZENIT_SCOPE_FUNCTION)
        {
            return zenit_scope_has_symbol(scope, symbol_name)
                    || zenit_scope_has_symbol(program->global_scope, symbol_name);
        }

        // In case the scope is not a function, keep searching
    } while (scope != NULL);
    
    return false;
}

ZenitSymbol* zenit_program_get_symbol(ZenitProgram *program, const char *symbol_name)
{
    if (program->current_scope->type == ZENIT_SCOPE_GLOBAL)
        return zenit_scope_get_symbol(program->current_scope, symbol_name);

    if (program->current_scope->type == ZENIT_SCOPE_FUNCTION)
    {
        return zenit_scope_has_symbol(program->current_scope, symbol_name) 
                    ? zenit_scope_get_symbol(program->current_scope, symbol_name)
                    : zenit_scope_has_symbol(program->global_scope, symbol_name)
                        ? zenit_scope_get_symbol(program->global_scope, symbol_name)
                        : NULL;
    }

    if (program->current_scope->type == ZENIT_SCOPE_STRUCT)
        return zenit_scope_get_symbol(program->current_scope, symbol_name);

    ZenitScope *scope = program->current_scope;
    do {
        if (zenit_scope_has_symbol(scope, symbol_name))
            return zenit_scope_get_symbol(scope, symbol_name);

        // If we couldn't find the symbol in the scope, try going to the parent scope
        scope = scope->parent;

        if (scope == NULL)
            break;

        // If the parent scope is a function, the only possible place the symbol could be
        // is within the function's scope or the global scope
        if (scope->type == ZENIT_SCOPE_FUNCTION)
        {
            return zenit_scope_has_symbol(scope, symbol_name) 
                    ? zenit_scope_get_symbol(scope, symbol_name)
                    : zenit_scope_has_symbol(program->global_scope, symbol_name)
                        ? zenit_scope_get_symbol(program->global_scope, symbol_name)
                        : NULL;
        }

        // In case the scope is not a function, keep searching
    } while (scope != NULL);
    
    return NULL;
}

ZenitSymbol* zenit_program_remove_symbol(ZenitProgram *program, const char *symbol_name)
{
    // FIXME: Fix this to lookup symbols in different scopes
    return zenit_symtable_remove(&program->current_scope->symtable, symbol_name);
}

char* zenit_program_dump(ZenitProgram *program, bool verbose)
{
    char *output = fl_cstring_dup("(program ");
    
    output = zenit_scope_dump(program->global_scope, output, verbose);
    
    fl_cstring_append(&output, ")");

    return output;
}

bool zenit_program_is_valid_type(ZenitProgram *program, ZenitType *type)
{
    if (type->typekind == ZENIT_TYPE_UINT || type->typekind == ZENIT_TYPE_BOOL)
        return true;

    if (type->typekind == ZENIT_TYPE_REFERENCE)
    {
        ZenitReferenceType *rti = (ZenitReferenceType*) type;
        return zenit_program_is_valid_type(program, rti->element);
    }

    if (type->typekind == ZENIT_TYPE_STRUCT)
    {
        ZenitStructType *struct_type = (ZenitStructType*) type;

        if (struct_type->name != NULL && !zenit_program_has_scope(program, ZENIT_SCOPE_STRUCT, struct_type->name))
            return false;

        struct FlListNode *tmp = fl_list_head(struct_type->members);

        while (tmp)
        {
            ZenitStructTypeMember *struct_member = (ZenitStructTypeMember*) tmp->value;

            if (!zenit_program_is_valid_type(program, struct_member->type))
                return false;

            tmp = tmp->next;
        }

        return true;
    }
    
    if (type->typekind == ZENIT_TYPE_ARRAY)
    {
        ZenitArrayType *ati = (ZenitArrayType*) type;
        return zenit_program_is_valid_type(program, ati->member_type);
    }

    return false;
}

ZenitType* zenit_program_get_invalid_type_component(ZenitProgram *program, ZenitType *type)
{
    // Cannot be undefined
    if (type->typekind == ZENIT_TYPE_UINT || type->typekind == ZENIT_TYPE_BOOL)
        return NULL;

    if (type->typekind == ZENIT_TYPE_REFERENCE)
    {
        ZenitReferenceType *rti = (ZenitReferenceType*) type;
        if (!zenit_program_is_valid_type(program, rti->element))
            return zenit_program_get_invalid_type_component(program, rti->element);

        return NULL;
    }

    if (type->typekind == ZENIT_TYPE_STRUCT)
    {
        ZenitStructType *struct_type = (ZenitStructType*) type;

        if (struct_type->name != NULL && !zenit_program_has_symbol(program, struct_type->name))
            return type;

        struct FlListNode *tmp = fl_list_head(struct_type->members);

        while (tmp)
        {
            ZenitStructTypeMember *struct_member = (ZenitStructTypeMember*) tmp->value;

            if (!zenit_program_is_valid_type(program, struct_member->type))
                return zenit_program_get_invalid_type_component(program, struct_member->type);

            tmp = tmp->next;
        }

        return NULL;
    }
    
    if (type->typekind == ZENIT_TYPE_ARRAY)
    {
        ZenitArrayType *ati = (ZenitArrayType*) type;
        if (!zenit_program_is_valid_type(program, ati->member_type))
            return zenit_program_get_invalid_type_component(program, ati->member_type);
        
        return NULL;
    }

    return NULL;
}
