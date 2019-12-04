#include "symtable.h"
#include "symbol.h"

struct ZirSymbolTable zir_symtable_new(enum ZirSymbolTableType type, const char *id)
{
    flm_assert(id != NULL, "Symbol table ID cannot be NULL");

    return (struct ZirSymbolTable) {
        .id = fl_cstring_dup(id),
        .type = type,
        .symbols = fl_hashtable_new_args((struct FlHashtableArgs) {
            .hash_function = fl_hashtable_hash_string,
            .key_allocator = fl_container_allocator_string,
            .key_comparer = fl_container_equals_string,
            .key_cleaner = fl_container_cleaner_pointer,
            .value_cleaner = (FlContainerCleanupFunction)zir_symbol_free,
            .value_allocator = NULL
        })
    };
}

void zir_symtable_free(struct ZirSymbolTable *symtable)
{
    if (!symtable)
        return;

    if (symtable->id)
        fl_cstring_free(symtable->id);

    if (symtable->symbols)
        fl_hashtable_free(symtable->symbols);
}

struct ZirSymbol* zir_symtable_add(struct ZirSymbolTable *symtable, struct ZirSymbol *symbol)
{
    fl_hashtable_add(symtable->symbols, symbol->name, symbol);
    return symbol;
}

bool zir_symtable_has(struct ZirSymbolTable *symtable, const char *symbol_name)
{
    return fl_hashtable_has_key(symtable->symbols, symbol_name);
}

struct ZirSymbol* zir_symtable_get(struct ZirSymbolTable *symtable, const char *symbol_name)
{
    return (struct ZirSymbol*)fl_hashtable_get(symtable->symbols, symbol_name);
}

struct ZirSymbol** zir_symtable_get_all(struct ZirSymbolTable *symtable)
{
    return (struct ZirSymbol**)fl_hashtable_values(symtable->symbols);
}
