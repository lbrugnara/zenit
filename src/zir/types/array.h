#ifndef ZIR_TYPE_ARRAY_H
#define ZIR_TYPE_ARRAY_H

#include <stdlib.h>
#include "type.h"

enum ZirArrayTypeSource {
    ZIR_ARRAY_TYPE_DECL,
    ZIR_ARRAY_TYPE_INFER,
};

struct ZirArrayType {
    struct ZirType base;
    enum ZirArrayTypeSource source;
    struct ZirType *member_type;
    size_t length;
};

struct ZirArrayType* zir_type_array_new(void);
unsigned long zir_type_array_hash(struct ZirArrayType *type);
struct ZirArrayType* zir_type_array_copy(struct ZirArrayType *source);
char* zir_type_array_to_string(struct ZirArrayType *type);
bool zir_type_array_equals(struct ZirArrayType *type_a, struct ZirType *type_b);
bool zir_type_array_is_assignable_from(struct ZirArrayType *target_type, struct ZirType *value_type);
bool zir_type_array_is_castable_to(struct ZirArrayType *array_type, struct ZirType *target_type);
size_t zir_type_array_size(struct ZirArrayType *type);
void zir_type_array_free(struct ZirArrayType *type);

#endif /* ZIR_TYPE_ARRAY_H */
