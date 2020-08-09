#include <fllib/Cstring.h>
#include "symtable.h"
#include "symbol.h"

ZirSymtable zir_symtable_new(void)
{
    return (ZirSymtable) {
        .names = fl_list_new_args((struct FlListArgs) {
            .value_allocator = fl_container_allocator_string,
            .value_cleaner = fl_container_cleaner_pointer
        }),
        .symbols = fl_hashtable_new_args((struct FlHashtableArgs) {
            .hash_function = fl_hashtable_hash_string,
            .key_allocator = fl_container_allocator_string,
            .key_comparer = fl_container_equals_string,
            .key_cleaner = fl_container_cleaner_pointer,
            .value_cleaner = (FlContainerCleanupFn)zir_symbol_free,
            .value_allocator = NULL
        })
    };
}

void zir_symtable_free(ZirSymtable *symtable)
{
    if (!symtable)
        return;

    if (symtable->symbols)
        fl_hashtable_free(symtable->symbols);
    
    if (symtable->names)
        fl_list_free(symtable->names);
}

ZirSymbol* zir_symtable_add(ZirSymtable *symtable, ZirSymbol *symbol)
{
    fl_list_append(symtable->names, symbol->name);
    fl_hashtable_add(symtable->symbols, symbol->name, symbol);
    return symbol;
}

bool zir_symtable_has(ZirSymtable *symtable, const char *symbol_name)
{
    return fl_hashtable_has_key(symtable->symbols, symbol_name);
}

ZirSymbol* zir_symtable_get(ZirSymtable *symtable, const char *symbol_name)
{
    return (ZirSymbol*)fl_hashtable_get(symtable->symbols, symbol_name);
}

ZirSymbol** zir_symtable_get_all(ZirSymtable *symtable)
{
    struct FlListNode *tmp = fl_list_head(symtable->names);

    if (tmp == NULL)
        return NULL;

    ZirSymbol **symbols = fl_array_new(sizeof(ZirSymbol*), 0);

    while (tmp)
    {
        char *name = (char*) tmp->value;

        ZirSymbol *symbol = zir_symtable_get(symtable, name);
        symbols = fl_array_append(symbols, &symbol);

        tmp = tmp->next;
    }

    return symbols;
}

char* zir_symtable_dump(ZirSymtable *symtable, char *output)
{
    struct FlListNode *tmp = fl_list_head(symtable->names);

    if (tmp == NULL)
        return output;

    bool started = false;
    while (tmp)
    {
        char *name = (char*) tmp->value;

        ZirSymbol *symbol = zir_symtable_get(symtable, name);

        if (started)
            fl_cstring_append(&output, ", ");

        started = true;

        output = zir_symbol_dump(symbol, output);

        tmp = tmp->next;
    }

    return output;
}
