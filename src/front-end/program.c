#include <fllib/Cstring.h>
#include "program.h"
#include "symbol.h"
#include "types/context.h"

struct ZenitProgram* zenit_program_new()
{
    struct ZenitProgram *program = fl_malloc(sizeof(struct ZenitProgram));
    program->global_scope = zenit_scope_new("global", ZENIT_SCOPE_GLOBAL, NULL);
    program->current_scope = program->global_scope;

    return program;
}

void zenit_program_free(struct ZenitProgram *program)
{
    if (!program)
        return;
        
    zenit_scope_free(program->global_scope);

    fl_free(program);
}

void zenit_program_add_scope(struct ZenitProgram *program, enum ZenitScopeType type, const char *name)
{
    struct ZenitScope *scope = zenit_scope_new(name, type, program->current_scope);
    program->current_scope->children = fl_array_append(program->current_scope->children, &scope);
}

bool zenit_program_enter_scope(struct ZenitProgram *program, struct ZenitScope *scope)
{
    if (scope->parent != program->current_scope)
        return false;

    program->current_scope = scope;
    return true;
}

void zenit_program_push_scope(struct ZenitProgram *program, enum ZenitScopeType type, const char *name)
{
    // First we search for an existent scope with the provided type and name    
    struct ZenitScope *scope = zenit_program_get_scope(program, type, name);

    if (scope == NULL)
    {
        // Add a new scope to the program
        scope = zenit_scope_new(name, type, program->current_scope);
        program->current_scope->children = fl_array_append(program->current_scope->children, &scope);
    }    

    program->current_scope = scope;
}

void zenit_program_pop_scope(struct ZenitProgram *program)
{
    program->current_scope = program->current_scope->parent;
}

bool zenit_program_has_scope(struct ZenitProgram *program, enum ZenitScopeType type, const char *name)
{
    // NOTE: For simplicity we use an array, we could replace this with a hashtable later
    struct ZenitScope *tmp_scope = program->current_scope;
    do
    {
        for (size_t i=0; i < fl_array_length(tmp_scope->children); i++)
        {
            struct ZenitScope *scope = tmp_scope->children[i];
            if (flm_cstring_equals(scope->id, name) && scope->type == type)
                return true;
        }
    
        if (type == ZENIT_SCOPE_STRUCT)
            tmp_scope = tmp_scope->parent;
        else break;

    } while (tmp_scope);

    return false;
}

struct ZenitScope* zenit_program_get_scope(struct ZenitProgram *program, enum ZenitScopeType type, const char *name)
{
    // NOTE: For simplicity we use an array, we could replace this with a hashtable later
    struct ZenitScope *tmp_scope = program->current_scope;
    do
    {
        for (size_t i=0; i < fl_array_length(tmp_scope->children); i++)
        {
            struct ZenitScope *scope = tmp_scope->children[i];
            if (flm_cstring_equals(scope->id, name) && scope->type == type)
                return scope;
        }

        if (type == ZENIT_SCOPE_STRUCT)
            tmp_scope = tmp_scope->parent;
        else break;

    } while (tmp_scope);    

    return NULL;
}

struct ZenitSymbol* zenit_program_add_symbol(struct ZenitProgram *program, struct ZenitSymbol *symbol)
{
    // FIXME: Fix this to add symbols to the global scope
    return zenit_symtable_add(&program->current_scope->symtable, symbol);
}

bool zenit_program_has_symbol(struct ZenitProgram *program, const char *symbol_name)
{
    // FIXME: Fix this to lookup symbols in different scopes
    return zenit_scope_has_symbol(program->current_scope, symbol_name);
}

struct ZenitSymbol* zenit_program_get_symbol(struct ZenitProgram *program, const char *symbol_name)
{
    // FIXME: Fix this to lookup symbols in different scopes
    return zenit_scope_get_symbol(program->current_scope, symbol_name);
}

struct ZenitSymbol* zenit_program_remove_symbol(struct ZenitProgram *program, const char *symbol_name)
{
    // FIXME: Fix this to lookup symbols in different scopes
    return zenit_symtable_remove(&program->current_scope->symtable, symbol_name);
}

char* zenit_program_dump(struct ZenitProgram *program, bool verbose)
{
    char *output = fl_cstring_dup("(program ");
    
    output = zenit_scope_dump(program->global_scope, output, verbose);
    
    fl_cstring_append(&output, ")");

    return output;
}

bool zenit_program_is_valid_type(struct ZenitProgram *program, struct ZenitType *type)
{
    if (type->typekind == ZENIT_TYPE_UINT || type->typekind == ZENIT_TYPE_BOOL)
        return true;

    if (type->typekind == ZENIT_TYPE_REFERENCE)
    {
        struct ZenitReferenceType *rti = (struct ZenitReferenceType*) type;
        return zenit_program_is_valid_type(program, rti->element);
    }

    if (type->typekind == ZENIT_TYPE_STRUCT)
    {
        struct ZenitStructType *struct_type = (struct ZenitStructType*) type;

        if (struct_type->name != NULL && !zenit_program_has_scope(program, ZENIT_SCOPE_STRUCT, struct_type->name))
            return false;

        struct FlListNode *tmp = fl_list_head(struct_type->members);

        while (tmp)
        {
            struct ZenitStructTypeMember *struct_member = (struct ZenitStructTypeMember*) tmp->value;

            if (!zenit_program_is_valid_type(program, struct_member->type))
                return false;

            tmp = tmp->next;
        }

        return true;
    }
    
    if (type->typekind == ZENIT_TYPE_ARRAY)
    {
        struct ZenitArrayType *ati = (struct ZenitArrayType*) type;
        return zenit_program_is_valid_type(program, ati->member_type);
    }

    return false;
}

struct ZenitType* zenit_program_get_invalid_type_component(struct ZenitProgram *program, struct ZenitType *type)
{
    // Cannot be undefined
    if (type->typekind == ZENIT_TYPE_UINT || type->typekind == ZENIT_TYPE_BOOL)
        return NULL;

    if (type->typekind == ZENIT_TYPE_REFERENCE)
    {
        struct ZenitReferenceType *rti = (struct ZenitReferenceType*) type;
        if (!zenit_program_is_valid_type(program, rti->element))
            return zenit_program_get_invalid_type_component(program, rti->element);

        return NULL;
    }

    if (type->typekind == ZENIT_TYPE_STRUCT)
    {
        struct ZenitStructType *struct_type = (struct ZenitStructType*) type;

        if (struct_type->name != NULL && !zenit_program_has_symbol(program, struct_type->name))
            return type;

        struct FlListNode *tmp = fl_list_head(struct_type->members);

        while (tmp)
        {
            struct ZenitStructTypeMember *struct_member = (struct ZenitStructTypeMember*) tmp->value;

            if (!zenit_program_is_valid_type(program, struct_member->type))
                return zenit_program_get_invalid_type_component(program, struct_member->type);

            tmp = tmp->next;
        }

        return NULL;
    }
    
    if (type->typekind == ZENIT_TYPE_ARRAY)
    {
        struct ZenitArrayType *ati = (struct ZenitArrayType*) type;
        if (!zenit_program_is_valid_type(program, ati->member_type))
            return zenit_program_get_invalid_type_component(program, ati->member_type);
        
        return NULL;
    }

    return NULL;
}
