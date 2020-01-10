#ifndef ZIR_TYPE_REFERENCE_H
#define ZIR_TYPE_REFERENCE_H

#include "type.h"

struct ZirReferenceTypeInfo {
    struct ZirTypeInfo base;
    struct ZirTypeInfo *element;
};

struct ZirReferenceTypeInfo* zir_type_reference_new(struct ZirTypeInfo *element);
unsigned long zir_type_reference_hash(struct ZirReferenceTypeInfo *typeinfo);
struct ZirReferenceTypeInfo* zir_type_reference_copy(struct ZirReferenceTypeInfo *src_type);
char* zir_type_reference_to_string(struct ZirReferenceTypeInfo *typeinfo);
bool zir_type_reference_equals(struct ZirReferenceTypeInfo *type_a, struct ZirTypeInfo *type_b);
bool zir_type_reference_is_assignable_from(struct ZirReferenceTypeInfo *target_type, struct ZirTypeInfo *value_type);
bool zir_type_reference_is_castable_to(struct ZirReferenceTypeInfo *reference, struct ZirTypeInfo *target_type);
size_t zir_type_reference_size(struct ZirReferenceTypeInfo *typeinfo);
void zir_type_reference_free(struct ZirReferenceTypeInfo *typeinfo);

#endif /* ZIR_TYPE_REFERENCE_H */
