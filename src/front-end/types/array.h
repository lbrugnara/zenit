#ifndef ZENIT_TYPE_ARRAY_H
#define ZENIT_TYPE_ARRAY_H

#include <stdlib.h>
#include "type.h"

enum ZenitArrayTypeInfoSource {
    ZENIT_ARRAY_TYPE_DECL,
    ZENIT_ARRAY_TYPE_INFER,
};

struct ZenitArrayTypeInfo {
    struct ZenitTypeInfo base;
    struct ZenitTypeInfo **members;
    enum ZenitArrayTypeInfoSource source;
    struct ZenitTypeInfo *member_type;
    size_t length;
};

struct ZenitArrayTypeInfo* zenit_type_array_new(struct ZenitTypeInfo *member_type);
void zenit_type_array_add_member(struct ZenitArrayTypeInfo *typeinfo, struct ZenitTypeInfo *element);
unsigned long zenit_type_array_hash(struct ZenitArrayTypeInfo *typeinfo);
struct ZenitArrayTypeInfo* zenit_type_array_copy(struct ZenitArrayTypeInfo *source);
char* zenit_type_array_to_string(struct ZenitArrayTypeInfo *typeinfo);
bool zenit_type_array_equals(struct ZenitArrayTypeInfo *type_a, struct ZenitTypeInfo *type_b);
bool zenit_type_array_is_assignable_from(struct ZenitArrayTypeInfo *target_type, struct ZenitTypeInfo *value_type);
bool zenit_type_array_is_castable_to(struct ZenitArrayTypeInfo *array_type, struct ZenitTypeInfo *target_type);
bool zenit_type_array_unify(struct ZenitArrayTypeInfo *array_type, struct ZenitTypeInfo *type_b, struct ZenitTypeInfo **unified);
void zenit_type_array_free(struct ZenitArrayTypeInfo *typeinfo);

#endif /* ZENIT_TYPE_ARRAY_H */
