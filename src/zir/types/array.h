#ifndef ZIR_TYPE_ARRAY_H
#define ZIR_TYPE_ARRAY_H

#include <stdlib.h>
#include "type.h"

enum ZirArrayTypeInfoSource {
    ZIR_ARRAY_TYPE_DECL,
    ZIR_ARRAY_TYPE_INFER,
};

struct ZirArrayTypeInfo {
    struct ZirTypeInfo base;
    enum ZirArrayTypeInfoSource source;
    struct ZirTypeInfo *member_type;
    size_t length;
};

struct ZirArrayTypeInfo* zir_type_array_new(void);
unsigned long zir_type_array_hash(struct ZirArrayTypeInfo *typeinfo);
struct ZirArrayTypeInfo* zir_type_array_copy(struct ZirArrayTypeInfo *source);
char* zir_type_array_to_string(struct ZirArrayTypeInfo *typeinfo);
bool zir_type_array_equals(struct ZirArrayTypeInfo *type_a, struct ZirTypeInfo *type_b);
bool zir_type_array_is_assignable_from(struct ZirArrayTypeInfo *target_type, struct ZirTypeInfo *value_type);
bool zir_type_array_is_castable_to(struct ZirArrayTypeInfo *array_type, struct ZirTypeInfo *target_type);
size_t zir_type_array_size(struct ZirArrayTypeInfo *typeinfo);
void zir_type_array_free(struct ZirArrayTypeInfo *typeinfo);

#endif /* ZIR_TYPE_ARRAY_H */
