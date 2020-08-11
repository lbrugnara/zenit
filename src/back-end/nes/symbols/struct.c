#include <fllib/Cstring.h>
#include <fllib/Array.h>
#include <fllib/containers/List.h>
#include "struct.h"
#include "../../../zir/types/struct.h"

ZnesStructSymbol* znes_symbol_struct_new(const char *name, ZirStructType *zir_struct_type, ZnesSegment segment, uint16_t address)
{
    ZnesStructSymbol *struct_symbol = fl_malloc(sizeof(ZnesStructSymbol));
    struct_symbol->base.address = address;
    struct_symbol->base.name = name != NULL ? fl_cstring_dup(name) : NULL;
    struct_symbol->base.segment = segment;
    struct_symbol->base.symkind = ZNES_SYMBOL_STRUCT;
    struct_symbol->base.size = zir_type_struct_size(zir_struct_type);
    struct_symbol->members = fl_array_new(sizeof(ZnesSymbol*), 0);
    
    size_t members_offset = 0;
    struct FlListNode *zir_node = fl_list_head(zir_struct_type->members);
    while (zir_node)
    {
        ZirStructTypeMember *zir_member = (ZirStructTypeMember*) zir_node->value;

        ZnesSymbol *member_symbol = znes_symbol_new(zir_member->name, zir_member->type, segment, address + members_offset);
        struct_symbol->members = fl_array_append(struct_symbol->members, &member_symbol);

        members_offset = zir_type_size(zir_member->type);

        zir_node = zir_node->next;
    }

    return struct_symbol;
}

void znes_symbol_struct_free(ZnesStructSymbol *struct_symbol)
{
    if (struct_symbol->base.name)
        fl_cstring_free(struct_symbol->base.name);

    fl_array_free_each_pointer(struct_symbol->members, (FlArrayFreeElementFunc) znes_symbol_free);

    fl_free(struct_symbol);
}
