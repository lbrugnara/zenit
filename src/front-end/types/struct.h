#ifndef ZENIT_TYPE_STRUCT_H
#define ZENIT_TYPE_STRUCT_H

#include <fllib.h>
#include "type.h"

struct ZenitStructTypeMember {
    const char *name;
    struct ZenitTypeInfo *typeinfo;
};

struct ZenitStructType {
    struct ZenitType base;
    char *name;
    FlList members;
};

struct ZenitStructType* zenit_type_struct_new(char *name);
void zenit_type_struct_add_member(struct ZenitStructType *type, const char *name, struct ZenitTypeInfo *typeifno);
unsigned long zenit_type_struct_hash(struct ZenitStructType *type);
char* zenit_type_struct_to_string(struct ZenitStructType *type);
bool zenit_type_struct_equals(struct ZenitStructType *type_a, struct ZenitType *type_b);
bool zenit_type_struct_structurally_equals(struct ZenitStructType *type_a, struct ZenitStructType *type_b);
bool zenit_type_struct_is_assignable_from(struct ZenitStructType *target_type, struct ZenitType *value_type);
bool zenit_type_struct_is_castable_to(struct ZenitStructType *struct_type, struct ZenitType *target_type);
bool zenit_type_struct_can_unify(struct ZenitStructType *struct_type, struct ZenitType *type_b);
void zenit_type_struct_free(struct ZenitStructType *type);

#endif /* ZENIT_TYPE_STRUCT_H */
