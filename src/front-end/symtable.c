#include "symtable.h"

CenitSymbolTable cenit_symtable_new(CenitSymbolTableType type, const char *id)
{
    flm_assert(id != NULL, "Symbol table ID cannot be NULL");

    return (CenitSymbolTable) {
        .id = fl_cstring_dup(id),
        .type = type,
        .symbols = fl_hashtable_new_args((struct FlHashtableArgs) {
            .hash_function = fl_hashtable_hash_string,
            .key_allocator = fl_container_allocator_string,
            .key_comparer = fl_container_equals_string,
            .key_cleaner = fl_container_cleaner_pointer,
            .value_cleaner = (FlContainerCleanupFunction)cenit_symbol_free,
            .value_allocator = NULL
        })
    };
}

void cenit_symtable_free(CenitSymbolTable *symtable)
{
    if (!symtable)
        return;

    if (symtable->id)
        fl_cstring_free(symtable->id);

    if (symtable->symbols)
        fl_hashtable_free(symtable->symbols);
}

CenitSymbol* cenit_symtable_add(CenitSymbolTable *symtable, CenitSymbol *symbol)
{
    fl_hashtable_add(symtable->symbols, symbol->name, symbol);
    return symbol;
}

bool cenit_symtable_has(CenitSymbolTable *symtable, const char *symbol_name)
{
    return fl_hashtable_has_key(symtable->symbols, symbol_name);
}

CenitSymbol* cenit_symtable_get(CenitSymbolTable *symtable, const char *symbol_name)
{
    return (CenitSymbol*)fl_hashtable_get(symtable->symbols, symbol_name);
}
