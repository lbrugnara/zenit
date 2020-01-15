#include "symtable.h"
#include "symbol.h"

struct ZenitSymtable zenit_symtable_new(void)
{
    return (struct ZenitSymtable) {
        .names = fl_list_new_args((struct FlListArgs) {
            .value_allocator = fl_container_allocator_string,
            .value_cleaner = fl_container_cleaner_pointer
        }),
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

    if (symtable->symbols)
        fl_hashtable_free(symtable->symbols);

    if (symtable->names)
        fl_list_free(symtable->names);
}

struct ZenitSymbol* zenit_symtable_add(struct ZenitSymtable *symtable, struct ZenitSymbol *symbol)
{
    fl_list_append(symtable->names, symbol->name);
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

bool zenit_symtable_is_empty(struct ZenitSymtable *symtable)
{
    return fl_hashtable_length(symtable->symbols) == 0;
}

char* zenit_symtable_dump(struct ZenitSymtable *symtable, char *output, bool verbose)
{
    struct FlListNode *tmp = fl_list_head(symtable->names);

    if (tmp == NULL)
        return output;

    bool started = false;
    while (tmp)
    {
        char *name = (char*) tmp->value;

        if (name[0] != '%' || verbose)
        {
            struct ZenitSymbol *symbol = zenit_symtable_get(symtable, name);

            if (started)
                fl_cstring_append(&output, " ");

            started = true;

            output = zenit_symbol_dump(symbol, output);
        }

        tmp = tmp->next;
    }

    return output;
}
