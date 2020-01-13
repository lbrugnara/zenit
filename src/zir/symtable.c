#include "symtable.h"
#include "symbol.h"

struct ZirSymbolTable zir_symtable_new(void)
{
    return (struct ZirSymbolTable) {
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

char* zir_symtable_dump(struct ZirSymbolTable *symtable, char *output)
{
    struct ZirSymbol **symbols = fl_hashtable_values(symtable->symbols);

    size_t length = fl_array_length(symbols);
    if (length > 0)
    {
        bool started = false;
        for (size_t i=0; i < length; i++)
        {
            if (started)
                fl_cstring_append(&output, ", ");

            started = true;

            output = zir_symbol_dump(symbols[i], output);            
        }
    }

    fl_array_free(symbols);

    return output;
}
