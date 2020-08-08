#include <fllib/Cstring.h>
#include "symtable.h"
#include "symbol.h"

struct ZirSymtable zir_symtable_new(void)
{
    return (struct ZirSymtable) {
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

void zir_symtable_free(struct ZirSymtable *symtable)
{
    if (!symtable)
        return;

    if (symtable->symbols)
        fl_hashtable_free(symtable->symbols);
    
    if (symtable->names)
        fl_list_free(symtable->names);
}

struct ZirSymbol* zir_symtable_add(struct ZirSymtable *symtable, struct ZirSymbol *symbol)
{
    fl_list_append(symtable->names, symbol->name);
    fl_hashtable_add(symtable->symbols, symbol->name, symbol);
    return symbol;
}

bool zir_symtable_has(struct ZirSymtable *symtable, const char *symbol_name)
{
    return fl_hashtable_has_key(symtable->symbols, symbol_name);
}

struct ZirSymbol* zir_symtable_get(struct ZirSymtable *symtable, const char *symbol_name)
{
    return (struct ZirSymbol*)fl_hashtable_get(symtable->symbols, symbol_name);
}

struct ZirSymbol** zir_symtable_get_all(struct ZirSymtable *symtable)
{
    struct FlListNode *tmp = fl_list_head(symtable->names);

    if (tmp == NULL)
        return NULL;

    struct ZirSymbol **symbols = fl_array_new(sizeof(struct ZirSymbol*), 0);

    while (tmp)
    {
        char *name = (char*) tmp->value;

        struct ZirSymbol *symbol = zir_symtable_get(symtable, name);
        symbols = fl_array_append(symbols, &symbol);

        tmp = tmp->next;
    }

    return symbols;
}

char* zir_symtable_dump(struct ZirSymtable *symtable, char *output)
{
    struct FlListNode *tmp = fl_list_head(symtable->names);

    if (tmp == NULL)
        return output;

    bool started = false;
    while (tmp)
    {
        char *name = (char*) tmp->value;

        struct ZirSymbol *symbol = zir_symtable_get(symtable, name);

        if (started)
            fl_cstring_append(&output, ", ");

        started = true;

        output = zir_symbol_dump(symbol, output);

        tmp = tmp->next;
    }

    return output;
}
