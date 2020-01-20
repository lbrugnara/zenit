#ifndef ZIR_TYPE_REFERENCE_H
#define ZIR_TYPE_REFERENCE_H

#include "type.h"

struct ZirReferenceType {
    struct ZirType base;
    struct ZirType *element;
};

struct ZirReferenceType* zir_type_reference_new(struct ZirType *element);
unsigned long zir_type_reference_hash(struct ZirReferenceType *type);
struct ZirReferenceType* zir_type_reference_copy(struct ZirReferenceType *src_type);
char* zir_type_reference_to_string(struct ZirReferenceType *type);
bool zir_type_reference_equals(struct ZirReferenceType *type_a, struct ZirType *type_b);
bool zir_type_reference_is_assignable_from(struct ZirReferenceType *target_type, struct ZirType *value_type);
bool zir_type_reference_is_castable_to(struct ZirReferenceType *reference, struct ZirType *target_type);
size_t zir_type_reference_size(struct ZirReferenceType *type);
void zir_type_reference_free(struct ZirReferenceType *type);

#endif /* ZIR_TYPE_REFERENCE_H */
