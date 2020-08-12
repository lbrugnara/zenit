#include <fllib/Cstring.h>
#include <fllib/Array.h>
#include <fllib/containers/List.h>
#include "struct.h"
#include "../operands/struct.h"
#include "../utils.h"

ZnesStructAlloc* znes_struct_alloc_new(const char *name, ZnesSegmentKind segment, size_t size, uint16_t address)
{
    ZnesStructAlloc *struct_symbol = fl_malloc(sizeof(ZnesStructAlloc));
    struct_symbol->base.address = address;
    struct_symbol->base.name = name != NULL ? fl_cstring_dup(name) : NULL;
    struct_symbol->base.segment = segment;
    struct_symbol->base.type = ZNES_ALLOC_STRUCT;
    struct_symbol->base.size = size;
    struct_symbol->members = fl_array_new(sizeof(ZnesAlloc*), 0);

    return struct_symbol;
}

ZnesAlloc* znes_struct_alloc_add_member(ZnesStructAlloc *struct_symbol, ZnesAlloc *member)
{
    struct_symbol->members = fl_array_append(struct_symbol->members, &member);
    return member;
}

void znes_struct_alloc_free(ZnesStructAlloc *struct_symbol)
{
    if (struct_symbol->base.name)
        fl_cstring_free(struct_symbol->base.name);

    fl_array_free_each_pointer(struct_symbol->members, (FlArrayFreeElementFunc) znes_alloc_free);

    fl_free(struct_symbol);
}
