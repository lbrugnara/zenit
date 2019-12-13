#ifndef ZIR_TYPE_STRUCT_H
#define ZIR_TYPE_STRUCT_H

#include "type.h"

struct ZirStructTypeInfo {
    struct ZirTypeInfo base;
    char *name;
    struct ZirTypeInfo *members;
};

struct ZirStructTypeInfo* zir_type_struct_new(char *name);
unsigned long zir_type_struct_hash(struct ZirStructTypeInfo *typeinfo);
struct ZirStructTypeInfo* zir_type_struct_copy(struct ZirStructTypeInfo *src_type);
char* zir_type_struct_to_string(struct ZirStructTypeInfo *typeinfo);
bool zir_type_struct_equals(struct ZirStructTypeInfo *type_a, struct ZirTypeInfo *type_b);
bool zir_type_struct_is_assignable_from(struct ZirStructTypeInfo *target_type, struct ZirTypeInfo *value_type);
bool zir_type_struct_is_castable_to(struct ZirStructTypeInfo *struct_type, struct ZirTypeInfo *target_type);
bool zir_type_struct_unify(struct ZirStructTypeInfo *struct_type, struct ZirTypeInfo *type_b, struct ZirTypeInfo **unified);
size_t zir_type_struct_size(struct ZirStructTypeInfo *typeinfo);
void zir_type_struct_free(struct ZirStructTypeInfo *typeinfo);

#endif /* ZIR_TYPE_STRUCT_H */
