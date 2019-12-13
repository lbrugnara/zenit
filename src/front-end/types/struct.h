#ifndef ZENIT_TYPE_STRUCT_H
#define ZENIT_TYPE_STRUCT_H

#include "type.h"

struct ZenitStructTypeInfo {
    struct ZenitTypeInfo base;
    char *name;
    struct ZenitTypeInfo *members;
};

struct ZenitStructTypeInfo* zenit_type_struct_new(enum ZenitTypeSource source, char *name);
unsigned long zenit_type_struct_hash(struct ZenitStructTypeInfo *typeinfo);
struct ZenitStructTypeInfo* zenit_type_struct_copy(struct ZenitStructTypeInfo *src_type);
char* zenit_type_struct_to_string(struct ZenitStructTypeInfo *typeinfo);
bool zenit_type_struct_equals(struct ZenitStructTypeInfo *type_a, struct ZenitTypeInfo *type_b);
bool zenit_type_struct_is_assignable_from(struct ZenitStructTypeInfo *target_type, struct ZenitTypeInfo *value_type);
bool zenit_type_struct_is_castable_to(struct ZenitStructTypeInfo *struct_type, struct ZenitTypeInfo *target_type);
bool zenit_type_struct_unify(struct ZenitStructTypeInfo *struct_type, struct ZenitTypeInfo *type_b, struct ZenitTypeInfo **unified);
void zenit_type_struct_free(struct ZenitStructTypeInfo *typeinfo);

#endif /* ZENIT_TYPE_STRUCT_H */
