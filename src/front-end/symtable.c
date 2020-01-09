#include "symtable.h"
#include "symbol.h"

struct ZenitSymtable zenit_symtable_new(enum ZenitSymtableType type, const char *id)
{
    flm_assert(id != NULL, "Symbol table ID cannot be NULL");

    return (struct ZenitSymtable) {
        .id = fl_cstring_dup(id),
        .type = type,
        .symbols = fl_hashtable_new_args((struct FlHashtableArgs) {
            .hash_function = fl_hashtable_hash_string,
            .key_allocator = fl_container_allocator_string,
            .key_comparer = fl_container_equals_string,
            .key_cleaner = fl_container_cleaner_pointer,
            .value_cleaner = (FlContainerCleanupFunction)zenit_symbol_free,
            .value_allocator = NULL
        })
    };
}

void zenit_symtable_free(struct ZenitSymtable *symtable)
{
    if (!symtable)
        return;

    if (symtable->id)
        fl_cstring_free(symtable->id);

    if (symtable->symbols)
        fl_hashtable_free(symtable->symbols);
}

struct ZenitSymbol* zenit_symtable_add(struct ZenitSymtable *symtable, struct ZenitSymbol *symbol)
{
    return (struct ZenitSymbol*) fl_hashtable_add(symtable->symbols, symbol->name, symbol);
}

bool zenit_symtable_has(struct ZenitSymtable *symtable, const char *symbol_name)
{
    return fl_hashtable_has_key(symtable->symbols, symbol_name);
}

struct ZenitSymbol* zenit_symtable_get(struct ZenitSymtable *symtable, const char *symbol_name)
{
    return (struct ZenitSymbol*) fl_hashtable_get(symtable->symbols, symbol_name);
}

struct ZenitSymbol* zenit_symtable_remove(struct ZenitSymtable *symtable, const char *symbol_name)
{
    struct ZenitSymbol *symbol = fl_hashtable_get(symtable->symbols, symbol_name);

    fl_hashtable_remove(symtable->symbols, symbol_name, true, false);

    return symbol;
}

char* zenit_symtable_dump(struct ZenitSymtable *symtable, char *output)
{
    if (symtable->type == ZENIT_SYMTABLE_GLOBAL)
        fl_cstring_append(&output, "GLOBAL\n");
    else
        fl_cstring_vappend(&output, "FUNCTION: %s\n", symtable->id);

    struct ZenitSymbol **symbols = fl_hashtable_values(symtable->symbols);

    for (size_t i=0; i < fl_array_length(symbols); i++)
    {
        if (symbols[i]->name[0] == '%')
            continue;
        output = zenit_symbol_dump(symbols[i], output);
    }

    fl_array_free(symbols);

    return output;
}
