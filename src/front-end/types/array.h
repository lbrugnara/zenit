#ifndef ZENIT_TYPE_ARRAY_H
#define ZENIT_TYPE_ARRAY_H

#include <stdlib.h>
#include "type.h"

struct ZenitArrayType {
    struct ZenitType base;
    struct ZenitTypeInfo *member_type;
    size_t length;
};

struct ZenitArrayType* zenit_type_array_new(struct ZenitTypeInfo *member_type);
unsigned long zenit_type_array_hash(struct ZenitArrayType *typeinfo);
char* zenit_type_array_to_string(struct ZenitArrayType *typeinfo);
bool zenit_type_array_equals(struct ZenitArrayType *type_a, struct ZenitType *type_b);
bool zenit_type_array_is_assignable_from(struct ZenitArrayType *target_type, struct ZenitType *value_type);
bool zenit_type_array_is_castable_to(struct ZenitArrayType *array_type, struct ZenitType *target_type);
bool zenit_type_array_can_unify(struct ZenitArrayType *array_type, struct ZenitType *type_b);
void zenit_type_array_free(struct ZenitArrayType *typeinfo);

#endif /* ZENIT_TYPE_ARRAY_H */
