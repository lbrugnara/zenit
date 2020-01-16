#ifndef ZENIT_TYPE_REFERENCE_H
#define ZENIT_TYPE_REFERENCE_H

#include "type.h"

struct ZenitReferenceType {
    struct ZenitType base;
    struct ZenitTypeInfo *element;
};

struct ZenitReferenceType* zenit_type_reference_new(struct ZenitTypeInfo *element);
unsigned long zenit_type_reference_hash(struct ZenitReferenceType *typeinfo);
struct ZenitReferenceType* zenit_type_reference_copy(struct ZenitReferenceType *src_type);
char* zenit_type_reference_to_string(struct ZenitReferenceType *typeinfo);
bool zenit_type_reference_equals(struct ZenitReferenceType *type_a, struct ZenitType *type_b);
bool zenit_type_reference_is_assignable_from(struct ZenitReferenceType *target_type, struct ZenitType *value_type);
bool zenit_type_reference_is_castable_to(struct ZenitReferenceType *reference, struct ZenitType *target_type);
struct ZenitTypeInfo* zenit_type_reference_unify(struct ZenitReferenceType *ref_type, struct ZenitType *type_b);
bool zenit_type_reference_can_unify(struct ZenitReferenceType *ref_type, struct ZenitType *type_b);
void zenit_type_reference_free(struct ZenitReferenceType *typeinfo);

#endif /* ZENIT_TYPE_REFERENCE_H */
