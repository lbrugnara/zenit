#include "system.h"

struct ZenitTypeSystem* zenit_typesys_new(void)
{
    struct ZenitTypeSystem *typesys = fl_malloc(sizeof(struct ZenitTypeSystem));

    typesys->pool = fl_hashtable_new_args((struct FlHashtableArgs) {
        .hash_function = fl_hashtable_hash_string,
        .key_allocator = fl_container_allocator_string,
        .key_comparer = fl_container_equals_string,
        .key_cleaner = fl_container_cleaner_pointer,
        .value_cleaner = (FlContainerCleanupFunction) zenit_type_free,
        .value_allocator = NULL
    });

    typesys->uniques = fl_list_new_args((struct FlListArgs) {
        .value_cleaner = (FlContainerCleanupFunction) zenit_type_free
    });
    
    return typesys;
}

struct ZenitArrayType* zenit_typesys_new_array(struct ZenitTypeSystem *typesys, struct ZenitTypeInfo *member_type)
{
    struct ZenitArrayType *array_type = zenit_type_array_new(member_type);

    fl_list_append(typesys->uniques, array_type);

    return array_type;
}

struct ZenitType* zenit_typesys_new_none(struct ZenitTypeSystem *typesys)
{
    struct ZenitType *none_type = NULL;

    if (fl_hashtable_has_key(typesys->pool, "none"))
    {
        none_type = fl_hashtable_get(typesys->pool, "none");
    }
    else
    {
        // First time
        none_type = zenit_type_none_new();
        fl_hashtable_add(typesys->pool, "none", none_type);
    }

    return none_type;
}

struct ZenitReferenceType* zenit_typesys_new_reference(struct ZenitTypeSystem *typesys, struct ZenitTypeInfo *element)
{
    struct ZenitReferenceType *ref_type = zenit_type_reference_new(element);

    fl_list_append(typesys->uniques, ref_type);

    return ref_type;
}

struct ZenitStructType* zenit_typesys_new_struct(struct ZenitTypeSystem *typesys, char *name)
{
    struct ZenitStructType *struct_type = NULL;

    if (name == NULL) // unnamed
    {
        struct_type = zenit_type_struct_new(NULL);
        fl_list_append(typesys->uniques, struct_type);
    }
    else
    {
        if (fl_hashtable_has_key(typesys->pool, name))
        {
            struct_type = fl_hashtable_get(typesys->pool, name);
        }
        else
        {
            struct_type = zenit_type_struct_new(name);
            fl_hashtable_add(typesys->pool, name, struct_type);
        }
    }

    return struct_type;
}

struct ZenitUintType* zenit_typesys_new_uint(struct ZenitTypeSystem *typesys, enum ZenitUintTypeSize size)
{
    const char *key = NULL;

    switch (size)
    {
        case ZENIT_UINT_8:
            key = "uint8";
            break;

        case ZENIT_UINT_16:
            key = "uint16";
            break;

        default:
            return NULL;
    }

    struct ZenitUintType *uint_type = NULL;

    if (fl_hashtable_has_key(typesys->pool, key))
    {
        uint_type = fl_hashtable_get(typesys->pool, key);
    }
    else
    {
        // First time
        uint_type = zenit_type_uint_new(size);
        fl_hashtable_add(typesys->pool, key, uint_type);
    }

    return uint_type;
}

struct ZenitArrayType* zenit_typesys_copy_array(struct ZenitTypeSystem *typesys, struct ZenitArrayType *src_array_type)
{
    struct ZenitTypeInfo *member_typeinfo = zenit_typeinfo_new(
                                                src_array_type->member_type->source, 
                                                src_array_type->member_type->sealed, 
                                                zenit_typesys_copy_type(typesys, src_array_type->member_type->type));

    struct ZenitArrayType *array_type = zenit_type_array_new(member_typeinfo);
    array_type->length = src_array_type->length;

    fl_list_append(typesys->uniques, array_type);

    return array_type;
}

struct ZenitReferenceType* zenit_typesys_copy_reference(struct ZenitTypeSystem *typesys, struct ZenitReferenceType *src_ref_type)
{
    struct ZenitTypeInfo *element_typeinfo = zenit_typeinfo_new(
                                                src_ref_type->element->source, 
                                                src_ref_type->element->sealed, 
                                                zenit_typesys_copy_type(typesys, src_ref_type->element->type));

    struct ZenitReferenceType *ref_type = zenit_type_reference_new(element_typeinfo);

    fl_list_append(typesys->uniques, ref_type);

    return ref_type;
}

struct ZenitStructType* zenit_typesys_copy_struct(struct ZenitTypeSystem *typesys, struct ZenitStructType *src_struct_type)
{
    // At this point src_struct_type should have been previously registered in the system
    // if not, we return NULL and it is an error
    if (src_struct_type->name != NULL)
        return fl_hashtable_get(typesys->pool, src_struct_type->name);


    struct ZenitStructType *struct_type = zenit_type_struct_new(NULL);

    struct FlListNode *src_node = fl_list_head(src_struct_type->members);

    while (src_node)
    {
        struct ZenitStructTypeMember *src_struct_member = (struct ZenitStructTypeMember*) src_node->value;

        struct ZenitTypeInfo *member_typeinfo = zenit_typeinfo_new(src_struct_member->typeinfo->source, 
                                                    src_struct_member->typeinfo->sealed, 
                                                    zenit_typesys_copy_type(typesys, src_struct_member->typeinfo->type));

        zenit_type_struct_add_member(struct_type, src_struct_member->name, member_typeinfo);

        src_node = src_node->next;
    }

    fl_list_append(typesys->uniques, struct_type);

    return struct_type;
}

struct ZenitUintType* zenit_typesys_copy_uint(struct ZenitTypeSystem *typesys, struct ZenitUintType *src_uint_type)
{
    const char *key = NULL;

    switch (src_uint_type->size)
    {
        case ZENIT_UINT_8:
            key = "uint8";
            break;

        case ZENIT_UINT_16:
            key = "uint16";
            break;

        default:
            return NULL;
    }

    struct ZenitUintType *uint_type = NULL;

    return fl_hashtable_get(typesys->pool, key);
}

struct ZenitType* zenit_typesys_copy_type(struct ZenitTypeSystem *typesys, struct ZenitType *type)
{
    if (type->typekind == ZENIT_TYPE_UINT)
        return (struct ZenitType*) zenit_typesys_copy_uint(typesys, (struct ZenitUintType*) type);

    if (type->typekind == ZENIT_TYPE_REFERENCE)
        return (struct ZenitType*) zenit_typesys_copy_reference(typesys, (struct ZenitReferenceType*) type);

    if (type->typekind == ZENIT_TYPE_STRUCT)
        return (struct ZenitType*) zenit_typesys_copy_struct(typesys, (struct ZenitStructType*) type);

    if (type->typekind == ZENIT_TYPE_ARRAY)
        return (struct ZenitType*) zenit_typesys_copy_array(typesys, (struct ZenitArrayType*) type);

    return zenit_typesys_new_none(typesys);
}

struct ZenitTypeInfo* zenit_type_array_unify(struct ZenitTypeSystem *typesys, struct ZenitArrayType *array_type, struct ZenitType *type_b)
{
    if (array_type == NULL || type_b == NULL)
        return NULL;

    if (type_b->typekind == ZENIT_TYPE_NONE)
        return zenit_typeinfo_new_array(ZENIT_TYPE_SRC_INFERRED, false, zenit_typesys_copy_array(typesys, array_type));

    if (type_b->typekind != ZENIT_TYPE_ARRAY)
        return NULL;

    if (zenit_type_array_equals(array_type, type_b))
        return zenit_typeinfo_new_array(ZENIT_TYPE_SRC_INFERRED, false, zenit_typesys_copy_array(typesys, array_type));

    struct ZenitArrayType *arr_type_b = (struct ZenitArrayType*) type_b;

    if (array_type->length != arr_type_b->length)
        return NULL;

    if (!zenit_type_can_unify(array_type->member_type->type, arr_type_b->member_type->type))
        return NULL;
    
    struct ZenitTypeInfo *unified_member_type = zenit_typesys_unify_type(typesys, array_type->member_type->type, arr_type_b->member_type->type);
    struct ZenitArrayType *unified_array = zenit_typesys_new_array(typesys, unified_member_type);
    unified_array->length = array_type->length;

    return zenit_typeinfo_new_array(ZENIT_TYPE_SRC_INFERRED, false, unified_array);
}

struct ZenitTypeInfo* zenit_type_reference_unify(struct ZenitTypeSystem *typesys, struct ZenitReferenceType *ref_type, struct ZenitType *type_b)
{
    if (ref_type == NULL || type_b == NULL)
        return NULL;

    if (type_b->typekind == ZENIT_TYPE_NONE)
        return zenit_typeinfo_new_reference(ZENIT_TYPE_SRC_INFERRED, false, zenit_typesys_copy_reference(typesys, ref_type));

    if (type_b->typekind != ZENIT_TYPE_REFERENCE)
        return NULL;

    if (zenit_type_reference_equals(ref_type, type_b))
        return zenit_typeinfo_new_reference(ZENIT_TYPE_SRC_INFERRED, false, zenit_typesys_copy_reference(typesys, ref_type));

    struct ZenitReferenceType *ref_type_b = (struct ZenitReferenceType*) type_b;

    if (!zenit_type_can_unify(ref_type->element->type, ref_type_b->element->type))
        return NULL;

    struct ZenitTypeInfo *unified_element_type = zenit_typesys_unify_type(typesys, ref_type->element->type, ref_type_b->element->type);
    struct ZenitTypeInfo *unified = zenit_typeinfo_new_reference(ZENIT_TYPE_SRC_INFERRED, false, zenit_typesys_new_reference(typesys, unified_element_type));

    return unified;
}

struct ZenitTypeInfo* zenit_type_struct_unify(struct ZenitTypeSystem *typesys, struct ZenitStructType *struct_type, struct ZenitType *type_b)
{
    if (struct_type == NULL || type_b == NULL)
        return NULL;

    if (type_b->typekind == ZENIT_TYPE_NONE)
        return zenit_typeinfo_new_struct(ZENIT_TYPE_SRC_INFERRED, false, zenit_typesys_copy_struct(typesys, struct_type));

    if (type_b->typekind != ZENIT_TYPE_STRUCT)
        return NULL;

    if (zenit_type_struct_equals(struct_type, type_b))
        return zenit_typeinfo_new_struct(ZENIT_TYPE_SRC_INFERRED, false, zenit_typesys_copy_struct(typesys, struct_type));

    struct ZenitStructType *struct_type_b = (struct ZenitStructType*) type_b;

    // If they are structurally equals, we just copy one of them and return an unnamed struct
    // (an unnamed struct can be casted or assigned to a named struct as long as they are structurally
    // equals and the other way around)
    if (zenit_type_struct_structurally_equals(struct_type, struct_type_b))
    {
        struct ZenitStructType *unified_struct = zenit_typesys_new_struct(typesys, NULL);

        struct FlListNode *struct_node = fl_list_head(struct_type->members);
        while (struct_node)
        {
            struct ZenitStructTypeMember *struct_member = (struct ZenitStructTypeMember*) struct_node->value;

            struct ZenitTypeInfo *member_typeinfo = zenit_typeinfo_new(struct_member->typeinfo->source, 
                                                        struct_member->typeinfo->sealed, 
                                                        zenit_typesys_copy_type(typesys, struct_member->typeinfo->type));

            zenit_type_struct_add_member(unified_struct, struct_member->name, member_typeinfo);

            struct_node = struct_node->next;
        }

        return zenit_typeinfo_new_struct(ZENIT_TYPE_SRC_INFERRED, false, unified_struct);
    }

    // FIXME: Let's assume we can't unify structurally inequal structs
    return NULL;
}

struct ZenitTypeInfo* zenit_type_uint_unify(struct ZenitTypeSystem *typesys, struct ZenitUintType *uint_type, struct ZenitType *type_b)
{
    if (uint_type == NULL || type_b == NULL)
        return NULL;

    if (type_b->typekind != ZENIT_TYPE_NONE && type_b->typekind != ZENIT_TYPE_UINT)
        return NULL;

    if (type_b->typekind == ZENIT_TYPE_NONE || zenit_type_uint_equals(uint_type, type_b))
    {
        struct ZenitTypeInfo *unified = zenit_typeinfo_new_uint(ZENIT_TYPE_SRC_INFERRED, false, 
                                            zenit_typesys_copy_uint(typesys, uint_type));
        return unified;
    }

    // At this point, type_b must be a uint
    struct ZenitUintType *uint_b = (struct ZenitUintType*) type_b;
    struct ZenitTypeInfo *unified = zenit_typeinfo_new_uint(ZENIT_TYPE_SRC_INFERRED, false, 
                                        zenit_typesys_copy_uint(typesys, uint_type->size > uint_b->size ? uint_type : uint_b));
    return unified;
}

struct ZenitTypeInfo* zenit_typesys_unify_type(struct ZenitTypeSystem *typesys, struct ZenitType *type_a, struct ZenitType *type_b)
{
    if (type_a == NULL || type_b == NULL)
        return NULL;

    if (type_a->typekind == ZENIT_TYPE_NONE)
    {
        if (type_b->typekind == ZENIT_TYPE_NONE)
            return NULL;

        return zenit_typeinfo_new(ZENIT_TYPE_SRC_INFERRED, false, zenit_typesys_copy_type(typesys, type_b));
    }

    if (type_a->typekind == ZENIT_TYPE_STRUCT)
        return zenit_type_struct_unify(typesys, (struct ZenitStructType*) type_a, type_b);
    
    if (type_a->typekind == ZENIT_TYPE_REFERENCE)
        return zenit_type_reference_unify(typesys, (struct ZenitReferenceType*) type_a, type_b);
    
    if (type_a->typekind == ZENIT_TYPE_ARRAY)
        return zenit_type_array_unify(typesys, (struct ZenitArrayType*) type_a, type_b);
    
    if (type_a->typekind == ZENIT_TYPE_UINT)
        return zenit_type_uint_unify(typesys, (struct ZenitUintType*) type_a, type_b);
    
    return NULL;
}

struct ZenitTypeInfo* zenit_typesys_copy_typeinfo(struct ZenitTypeSystem *typesys, struct ZenitTypeInfo *typeinfo)
{
    return zenit_typeinfo_new(typeinfo->source, typeinfo->sealed, zenit_typesys_copy_type(typesys, typeinfo->type));
}

void zenit_typesys_free(struct ZenitTypeSystem *system)
{
    fl_list_free(system->uniques);
    fl_hashtable_free(system->pool);
    fl_free(system);
}
