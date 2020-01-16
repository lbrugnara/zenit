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

struct ZenitArrayType* zenit_typesys_new_array(struct ZenitTypeSystem *typesys, struct ZenitType *member_type)
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

struct ZenitReferenceType* zenit_typesys_new_reference(struct ZenitTypeSystem *typesys, struct ZenitType *element)
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
    struct ZenitArrayType *array_type = zenit_type_array_new(zenit_typesys_copy_type(typesys, src_array_type->member_type));
    array_type->length = src_array_type->length;

    fl_list_append(typesys->uniques, array_type);

    return array_type;
}

struct ZenitReferenceType* zenit_typesys_copy_reference(struct ZenitTypeSystem *typesys, struct ZenitReferenceType *src_ref_type)
{
    struct ZenitReferenceType *ref_type = zenit_type_reference_new(zenit_typesys_copy_type(typesys, src_ref_type->element));

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

        zenit_type_struct_add_member(struct_type, src_struct_member->name, zenit_typesys_copy_type(typesys, src_struct_member->type));

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

bool zenit_typesys_unify_array(struct ZenitTypeSystem *typesys, struct ZenitArrayType *array_type, struct ZenitType *type_b, struct ZenitTypeInfo *dest)
{
    if (array_type == NULL || type_b == NULL)
        return false;

    if (type_b->typekind == ZENIT_TYPE_NONE)
    {
        dest->source = ZENIT_TYPE_SRC_INFERRED;
        dest->sealed = false;
        dest->type = (struct ZenitType*) zenit_typesys_copy_array(typesys, array_type);
        return true;
    }

    if (type_b->typekind != ZENIT_TYPE_ARRAY)
        return false;

    if (zenit_type_array_equals(array_type, type_b))
    {
        dest->source = ZENIT_TYPE_SRC_INFERRED;
        dest->sealed = false;
        dest->type = (struct ZenitType*) zenit_typesys_copy_array(typesys, array_type);
        return true;
    }

    struct ZenitArrayType *arr_type_b = (struct ZenitArrayType*) type_b;

    if (array_type->length != arr_type_b->length)
        return false;

    if (!zenit_type_can_unify(array_type->member_type, arr_type_b->member_type))
        return false;
    
    struct ZenitTypeInfo unified_member_type = { 0 };
    
    if (!zenit_typesys_unify_types(typesys, array_type->member_type, arr_type_b->member_type, &unified_member_type))
        return false;

    struct ZenitArrayType *unified_array = zenit_typesys_new_array(typesys, unified_member_type.type);
    unified_array->length = array_type->length;

    dest->source = ZENIT_TYPE_SRC_INFERRED;
    dest->sealed = false;
    dest->type = (struct ZenitType*) unified_array;

    return true;
}

bool zenit_typesys_unify_reference(struct ZenitTypeSystem *typesys, struct ZenitReferenceType *ref_type, struct ZenitType *type_b, struct ZenitTypeInfo *dest)
{
    if (ref_type == NULL || type_b == NULL)
        return false;

    if (type_b->typekind == ZENIT_TYPE_NONE)
    {
        dest->source = ZENIT_TYPE_SRC_INFERRED;
        dest->sealed = false;
        dest->type = (struct ZenitType*) zenit_typesys_copy_reference(typesys, ref_type);
        return true;
    }

    if (type_b->typekind != ZENIT_TYPE_REFERENCE)
        return false;

    if (zenit_type_reference_equals(ref_type, type_b))
    {
        dest->source = ZENIT_TYPE_SRC_INFERRED;
        dest->sealed = false;
        dest->type = (struct ZenitType*) zenit_typesys_copy_reference(typesys, ref_type);
        return true;
    }

    struct ZenitReferenceType *ref_type_b = (struct ZenitReferenceType*) type_b;

    if (!zenit_type_can_unify(ref_type->element, ref_type_b->element))
        return false;

    struct ZenitTypeInfo unified_element_type = { 0 };

    if (!zenit_typesys_unify_types(typesys, ref_type->element, ref_type_b->element, &unified_element_type))
        return false;

    dest->source = ZENIT_TYPE_SRC_INFERRED;
    dest->sealed = false;
    dest->type = (struct ZenitType*) zenit_typesys_new_reference(typesys, unified_element_type.type);

    return true;
}

bool zenit_typesys_unify_struct(struct ZenitTypeSystem *typesys, struct ZenitStructType *struct_type, struct ZenitType *type_b, struct ZenitTypeInfo *dest)
{
    if (struct_type == NULL || type_b == NULL)
        return false;

    if (type_b->typekind == ZENIT_TYPE_NONE)
    {
        dest->source = ZENIT_TYPE_SRC_INFERRED;
        dest->sealed = false;
        dest->type = (struct ZenitType*) zenit_typesys_copy_struct(typesys, struct_type);
        return true;
    }

    if (type_b->typekind != ZENIT_TYPE_STRUCT)
        return false;

    if (zenit_type_struct_equals(struct_type, type_b))
    {
        dest->source = ZENIT_TYPE_SRC_INFERRED;
        dest->sealed = false;
        dest->type = (struct ZenitType*) zenit_typesys_copy_struct(typesys, struct_type);
        return true;
    }

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

            zenit_type_struct_add_member(unified_struct, struct_member->name, zenit_typesys_copy_type(typesys, struct_member->type));

            struct_node = struct_node->next;
        }

        dest->source = ZENIT_TYPE_SRC_INFERRED;
        dest->sealed = false;
        dest->type = (struct ZenitType*) unified_struct;
        return true;
    }

    // FIXME: Let's assume we can't unify structurally inequal structs
    return false;
}

bool zenit_typesys_unify_uint(struct ZenitTypeSystem *typesys, struct ZenitUintType *uint_type, struct ZenitType *type_b, struct ZenitTypeInfo *dest)
{
    if (uint_type == NULL || type_b == NULL)
        return false;

    if (type_b->typekind != ZENIT_TYPE_NONE && type_b->typekind != ZENIT_TYPE_UINT)
        return false;

    if (type_b->typekind == ZENIT_TYPE_NONE || zenit_type_uint_equals(uint_type, type_b))
    {
        dest->source = ZENIT_TYPE_SRC_INFERRED;
        dest->sealed = false;
        dest->type = (struct ZenitType*) zenit_typesys_copy_uint(typesys, uint_type);
        return true;
    }

    // At this point, type_b must be a uint
    struct ZenitUintType *uint_b = (struct ZenitUintType*) type_b;
    dest->source = ZENIT_TYPE_SRC_INFERRED;
    dest->sealed = false;
    dest->type = (struct ZenitType*) zenit_typesys_copy_uint(typesys, uint_type->size > uint_b->size ? uint_type : uint_b);
    return true;
}

bool zenit_typesys_unify_types(struct ZenitTypeSystem *typesys, struct ZenitType *type_a, struct ZenitType *type_b, struct ZenitTypeInfo *dest)
{
    if (type_a == NULL || type_b == NULL)
        return false;

    if (type_a->typekind == ZENIT_TYPE_NONE)
    {
        if (type_b->typekind == ZENIT_TYPE_NONE)
            return false;

        dest->source = ZENIT_TYPE_SRC_INFERRED;
        dest->sealed = false;
        dest->type = zenit_typesys_copy_type(typesys, type_b);
        return true;
    }

    if (type_a->typekind == ZENIT_TYPE_STRUCT)
        return zenit_typesys_unify_struct(typesys, (struct ZenitStructType*) type_a, type_b, dest);
    
    if (type_a->typekind == ZENIT_TYPE_REFERENCE)
        return zenit_typesys_unify_reference(typesys, (struct ZenitReferenceType*) type_a, type_b, dest);
    
    if (type_a->typekind == ZENIT_TYPE_ARRAY)
        return zenit_typesys_unify_array(typesys, (struct ZenitArrayType*) type_a, type_b, dest);
    
    if (type_a->typekind == ZENIT_TYPE_UINT)
        return zenit_typesys_unify_uint(typesys, (struct ZenitUintType*) type_a, type_b, dest);
    
    return false;
}

void zenit_typesys_free(struct ZenitTypeSystem *system)
{
    fl_list_free(system->uniques);
    fl_hashtable_free(system->pool);
    fl_free(system);
}
