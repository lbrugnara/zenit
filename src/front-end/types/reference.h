#ifndef ZENIT_TYPE_REFERENCE_H
#define ZENIT_TYPE_REFERENCE_H

#include "type.h"

struct ZenitReferenceTypeInfo {
    struct ZenitTypeInfo base;
    struct ZenitTypeInfo *element;
};

struct ZenitReferenceTypeInfo* zenit_type_reference_new(struct ZenitTypeInfo *element);
unsigned long zenit_type_reference_hash(struct ZenitReferenceTypeInfo *typeinfo);
struct ZenitReferenceTypeInfo* zenit_type_reference_copy(struct ZenitReferenceTypeInfo *src_type);
char* zenit_type_reference_to_string(struct ZenitReferenceTypeInfo *typeinfo);
bool zenit_type_reference_equals(struct ZenitReferenceTypeInfo *type_a, struct ZenitTypeInfo *type_b);
bool zenit_type_reference_is_assignable_from(struct ZenitReferenceTypeInfo *target_type, struct ZenitTypeInfo *value_type);
bool zenit_type_reference_is_castable_to(struct ZenitReferenceTypeInfo *reference, struct ZenitTypeInfo *target_type);
bool zenit_type_reference_unify(struct ZenitReferenceTypeInfo *ref_type, struct ZenitTypeInfo *type_b, struct ZenitTypeInfo **unified);
void zenit_type_reference_free(struct ZenitReferenceTypeInfo *typeinfo);

#endif /* ZENIT_TYPE_REFERENCE_H */
