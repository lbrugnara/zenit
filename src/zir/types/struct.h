#ifndef ZIR_TYPE_STRUCT_H
#define ZIR_TYPE_STRUCT_H

#include "type.h"

struct ZirStructType {
    struct ZirType base;
    char *name;
    struct ZirType *members;
};

struct ZirStructType* zir_type_struct_new(char *name);
unsigned long zir_type_struct_hash(struct ZirStructType *type);
struct ZirStructType* zir_type_struct_copy(struct ZirStructType *src_type);
char* zir_type_struct_to_string(struct ZirStructType *type);
bool zir_type_struct_equals(struct ZirStructType *type_a, struct ZirType *type_b);
bool zir_type_struct_is_assignable_from(struct ZirStructType *target_type, struct ZirType *value_type);
bool zir_type_struct_is_castable_to(struct ZirStructType *struct_type, struct ZirType *target_type);
size_t zir_type_struct_size(struct ZirStructType *type);
void zir_type_struct_free(struct ZirStructType *type);

#endif /* ZIR_TYPE_STRUCT_H */
