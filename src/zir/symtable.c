#include "symtable.h"
#include "symbol.h"

struct ZenitIrSymbolTable zenit_ir_symtable_new(enum ZenitIrSymbolTableType type, const char *id)
{
    flm_assert(id != NULL, "Symbol table ID cannot be NULL");

    return (struct ZenitIrSymbolTable) {
        .id = fl_cstring_dup(id),
        .type = type,
        .symbols = fl_hashtable_new_args((struct FlHashtableArgs) {
            .hash_function = fl_hashtable_hash_string,
            .key_allocator = fl_container_allocator_string,
            .key_comparer = fl_container_equals_string,
            .key_cleaner = fl_container_cleaner_pointer,
            .value_cleaner = (FlContainerCleanupFunction)zenit_ir_symbol_free,
            .value_allocator = NULL
        })
    };
}

void zenit_ir_symtable_free(struct ZenitIrSymbolTable *symtable)
{
    if (!symtable)
        return;

    if (symtable->id)
        fl_cstring_free(symtable->id);

    if (symtable->symbols)
        fl_hashtable_free(symtable->symbols);
}

struct ZenitIrSymbol* zenit_ir_symtable_add(struct ZenitIrSymbolTable *symtable, struct ZenitIrSymbol *symbol)
{
    fl_hashtable_add(symtable->symbols, symbol->name, symbol);
    return symbol;
}

bool zenit_ir_symtable_has(struct ZenitIrSymbolTable *symtable, const char *symbol_name)
{
    return fl_hashtable_has_key(symtable->symbols, symbol_name);
}

struct ZenitIrSymbol* zenit_ir_symtable_get(struct ZenitIrSymbolTable *symtable, const char *symbol_name)
{
    return (struct ZenitIrSymbol*)fl_hashtable_get(symtable->symbols, symbol_name);
}

struct ZenitIrSymbol** zenit_ir_symtable_get_all(struct ZenitIrSymbolTable *symtable)
{
    return (struct ZenitIrSymbol**)fl_hashtable_values(symtable->symbols);
}
