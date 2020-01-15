#ifndef ZENIT_TYPE_STRUCT_H
#define ZENIT_TYPE_STRUCT_H

#include <fllib.h>
#include "type.h"

struct ZenitStructTypeInfo {
    struct ZenitTypeInfo base;
    char *name;
    FlList members;
};

struct ZenitStructTypeInfo* zenit_type_struct_new(enum ZenitTypeSource source, char *name);
void zenit_type_struct_add_member(struct ZenitStructTypeInfo *typeinfo, const char *name, struct ZenitTypeInfo *type);
unsigned long zenit_type_struct_hash(struct ZenitStructTypeInfo *typeinfo);
struct ZenitStructTypeInfo* zenit_type_struct_copy(struct ZenitStructTypeInfo *src_type);
char* zenit_type_struct_to_string(struct ZenitStructTypeInfo *typeinfo);
bool zenit_type_struct_equals(struct ZenitStructTypeInfo *type_a, struct ZenitTypeInfo *type_b);
bool zenit_type_struct_is_assignable_from(struct ZenitStructTypeInfo *target_type, struct ZenitTypeInfo *value_type);
bool zenit_type_struct_is_castable_to(struct ZenitStructTypeInfo *struct_type, struct ZenitTypeInfo *target_type);
struct ZenitTypeInfo* zenit_type_struct_unify(struct ZenitStructTypeInfo *struct_type, struct ZenitTypeInfo *type_b);
bool zenit_type_struct_can_unify(struct ZenitStructTypeInfo *struct_type, struct ZenitTypeInfo *type_b);
void zenit_type_struct_free(struct ZenitStructTypeInfo *typeinfo);

#endif /* ZENIT_TYPE_STRUCT_H */
