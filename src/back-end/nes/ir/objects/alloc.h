#ifndef ZNES_ALLOC_H
#define ZNES_ALLOC_H

#include <stdint.h>
#include <fllib/containers/Hashtable.h>
#include <fllib/containers/List.h>
#include "../segment.h"
#include "../operands/operand.h"

typedef FlHashtable ZnesVariableMap;
typedef FlList ZnesVariableList;

typedef enum ZnesAllocType {
    ZNES_ALLOC_UNK,
    ZNES_ALLOC_UINT,
    ZNES_ALLOC_BOOL,
    ZNES_ALLOC_ARRAY,
    ZNES_ALLOC_STRUCT,
    ZNES_ALLOC_REFERENCE,
    ZNES_ALLOC_TEMP,
} ZnesAllocType;

typedef struct ZnesAllocRequest {
    ZnesSegmentKind segment;
    ZnesAllocType kind;
    size_t size;
    uint16_t address;
    bool use_address;
    bool is_global;
} ZnesAllocRequest;

typedef struct ZnesAlloc {
    ZnesAllocType type;
    ZnesSegmentKind segment;
    char *name;
    uint16_t address;
    size_t size;
} ZnesAlloc;

ZnesAlloc* znes_alloc_new(ZnesAllocType type, const char *name, ZnesSegmentKind segment, size_t size, uint16_t address);
void znes_alloc_free(ZnesAlloc *symbol);

#endif /* ZNES_ALLOC_H */
