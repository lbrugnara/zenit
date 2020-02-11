#include "context.h"

struct ZenitTypeContext* zenit_type_ctx_new(void)
{
    struct ZenitTypeContext *type_ctx = fl_malloc(sizeof(struct ZenitTypeContext));

    type_ctx->pool = fl_hashtable_new_args((struct FlHashtableArgs) {
        .hash_function = fl_hashtable_hash_string,
        .key_allocator = fl_container_allocator_string,
        .key_comparer = fl_container_equals_string,
        .key_cleaner = fl_container_cleaner_pointer,
        .value_cleaner = (FlContainerCleanupFunction) zenit_type_free,
        .value_allocator = NULL
    });

    type_ctx->uniques = fl_list_new_args((struct FlListArgs) {
        .value_cleaner = (FlContainerCleanupFunction) zenit_type_free
    });
    
    return type_ctx;
}

struct ZenitArrayType* zenit_type_ctx_new_array(struct ZenitTypeContext *type_ctx, struct ZenitType *member_type)
{
    struct ZenitArrayType *array_type = zenit_type_array_new(member_type);

    fl_list_append(type_ctx->uniques, array_type);

    return array_type;
}

struct ZenitType* zenit_type_ctx_new_none(struct ZenitTypeContext *type_ctx)
{
    struct ZenitType *none_type = NULL;

    if (fl_hashtable_has_key(type_ctx->pool, "none"))
    {
        none_type = fl_hashtable_get(type_ctx->pool, "none");
    }
    else
    {
        // First time
        none_type = zenit_type_none_new();
        fl_hashtable_add(type_ctx->pool, "none", none_type);
    }

    return none_type;
}

struct ZenitReferenceType* zenit_type_ctx_new_reference(struct ZenitTypeContext *type_ctx, struct ZenitType *element)
{
    struct ZenitReferenceType *ref_type = zenit_type_reference_new(element);

    fl_list_append(type_ctx->uniques, ref_type);

    return ref_type;
}

struct ZenitStructType* zenit_type_ctx_new_struct(struct ZenitTypeContext *type_ctx, char *name)
{
    struct ZenitStructType *struct_type = NULL;

    if (name == NULL) // unnamed
    {
        struct_type = zenit_type_struct_new(NULL);
        fl_list_append(type_ctx->uniques, struct_type);
    }
    else
    {
        if (fl_hashtable_has_key(type_ctx->pool, name))
        {
            struct_type = fl_hashtable_get(type_ctx->pool, name);
        }
        else
        {
            struct_type = zenit_type_struct_new(name);
            fl_hashtable_add(type_ctx->pool, name, struct_type);
        }
    }

    return struct_type;
}

struct ZenitStructType* zenit_type_ctx_get_named_struct(struct ZenitTypeContext *type_ctx, char *name)
{
    if (fl_hashtable_has_key(type_ctx->pool, name))
        return fl_hashtable_get(type_ctx->pool, name);

    return NULL;
}

struct ZenitUintType* zenit_type_ctx_new_uint(struct ZenitTypeContext *type_ctx, enum ZenitUintTypeSize size)
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

    if (fl_hashtable_has_key(type_ctx->pool, key))
    {
        uint_type = fl_hashtable_get(type_ctx->pool, key);
    }
    else
    {
        // First time
        uint_type = zenit_type_uint_new(size);
        fl_hashtable_add(type_ctx->pool, key, uint_type);
    }

    return uint_type;
}

struct ZenitBoolType* zenit_type_ctx_new_bool(struct ZenitTypeContext *type_ctx)
{
    struct ZenitBoolType *bool_type = NULL;

    if (fl_hashtable_has_key(type_ctx->pool, "bool"))
    {
        bool_type = fl_hashtable_get(type_ctx->pool, "bool");
    }
    else
    {
        // First time
        bool_type = zenit_type_bool_new();
        fl_hashtable_add(type_ctx->pool, "bool", bool_type);
    }

    return bool_type;
}

struct ZenitArrayType* zenit_type_ctx_copy_array(struct ZenitTypeContext *type_ctx, struct ZenitArrayType *src_array_type)
{
    struct ZenitArrayType *array_type = zenit_type_array_new(zenit_type_ctx_copy_type(type_ctx, src_array_type->member_type));
    array_type->length = src_array_type->length;

    fl_list_append(type_ctx->uniques, array_type);

    return array_type;
}

struct ZenitReferenceType* zenit_type_ctx_copy_reference(struct ZenitTypeContext *type_ctx, struct ZenitReferenceType *src_ref_type)
{
    struct ZenitReferenceType *ref_type = zenit_type_reference_new(zenit_type_ctx_copy_type(type_ctx, src_ref_type->element));

    fl_list_append(type_ctx->uniques, ref_type);

    return ref_type;
}

struct ZenitStructType* zenit_type_ctx_copy_struct(struct ZenitTypeContext *type_ctx, struct ZenitStructType *src_struct_type)
{
    // At this point src_struct_type should have been previously registered in the system
    // if not, we return NULL and it is an error
    if (src_struct_type->name != NULL)
        return fl_hashtable_get(type_ctx->pool, src_struct_type->name);


    struct ZenitStructType *struct_type = zenit_type_struct_new(NULL);

    struct FlListNode *src_node = fl_list_head(src_struct_type->members);

    while (src_node)
    {
        struct ZenitStructTypeMember *src_struct_member = (struct ZenitStructTypeMember*) src_node->value;

        zenit_type_struct_add_member(struct_type, src_struct_member->name, zenit_type_ctx_copy_type(type_ctx, src_struct_member->type));

        src_node = src_node->next;
    }

    fl_list_append(type_ctx->uniques, struct_type);

    return struct_type;
}

struct ZenitUintType* zenit_type_ctx_copy_uint(struct ZenitTypeContext *type_ctx, struct ZenitUintType *src_uint_type)
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

    return fl_hashtable_get(type_ctx->pool, key);
}

struct ZenitBoolType* zenit_type_ctx_copy_bool(struct ZenitTypeContext *type_ctx, struct ZenitBoolType *src_bool_type)
{
    return fl_hashtable_get(type_ctx->pool, "bool");
}

struct ZenitType* zenit_type_ctx_copy_type(struct ZenitTypeContext *type_ctx, struct ZenitType *type)
{
    if (type->typekind == ZENIT_TYPE_UINT)
        return (struct ZenitType*) zenit_type_ctx_copy_uint(type_ctx, (struct ZenitUintType*) type);

    if (type->typekind == ZENIT_TYPE_BOOL)
        return (struct ZenitType*) zenit_type_ctx_copy_bool(type_ctx, (struct ZenitBoolType*) type);

    if (type->typekind == ZENIT_TYPE_REFERENCE)
        return (struct ZenitType*) zenit_type_ctx_copy_reference(type_ctx, (struct ZenitReferenceType*) type);

    if (type->typekind == ZENIT_TYPE_STRUCT)
        return (struct ZenitType*) zenit_type_ctx_copy_struct(type_ctx, (struct ZenitStructType*) type);

    if (type->typekind == ZENIT_TYPE_ARRAY)
        return (struct ZenitType*) zenit_type_ctx_copy_array(type_ctx, (struct ZenitArrayType*) type);

    return zenit_type_ctx_new_none(type_ctx);
}

static bool zenit_type_ctx_unify_array(struct ZenitTypeContext *type_ctx, struct ZenitArrayType *array_type, struct ZenitType *type_b, struct ZenitType **dest)
{
    if (array_type == NULL || type_b == NULL)
        return false;

    if (type_b->typekind == ZENIT_TYPE_NONE)
    {
        *dest = (struct ZenitType*) zenit_type_ctx_copy_array(type_ctx, array_type);
        return true;
    }

    if (type_b->typekind != ZENIT_TYPE_ARRAY)
        return false;

    if (zenit_type_array_equals(array_type, type_b))
    {
        *dest = (struct ZenitType*) zenit_type_ctx_copy_array(type_ctx, array_type);
        return true;
    }

    struct ZenitArrayType *arr_type_b = (struct ZenitArrayType*) type_b;

    if (array_type->length != arr_type_b->length)
        return false;

    if (!zenit_type_can_unify(array_type->member_type, arr_type_b->member_type))
        return false;
    
    struct ZenitType *unified_member_type = NULL;
    
    if (!zenit_type_ctx_unify_types(type_ctx, array_type->member_type, arr_type_b->member_type, &unified_member_type))
        return false;

    struct ZenitArrayType *unified_array = zenit_type_ctx_new_array(type_ctx, unified_member_type);
    unified_array->length = array_type->length;

    *dest = (struct ZenitType*) unified_array;

    return true;
}

static bool zenit_type_ctx_unify_reference(struct ZenitTypeContext *type_ctx, struct ZenitReferenceType *ref_type, struct ZenitType *type_b, struct ZenitType **dest)
{
    if (ref_type == NULL || type_b == NULL)
        return false;

    if (type_b->typekind == ZENIT_TYPE_NONE)
    {
        *dest = (struct ZenitType*) zenit_type_ctx_copy_reference(type_ctx, ref_type);
        return true;
    }

    if (type_b->typekind != ZENIT_TYPE_REFERENCE)
        return false;

    if (zenit_type_reference_equals(ref_type, type_b))
    {
        *dest = (struct ZenitType*) zenit_type_ctx_copy_reference(type_ctx, ref_type);
        return true;
    }

    struct ZenitReferenceType *ref_type_b = (struct ZenitReferenceType*) type_b;

    if (!zenit_type_can_unify(ref_type->element, ref_type_b->element))
        return false;

    struct ZenitType *unified_element_type = NULL;

    if (!zenit_type_ctx_unify_types(type_ctx, ref_type->element, ref_type_b->element, &unified_element_type))
        return false;

    *dest = (struct ZenitType*) zenit_type_ctx_new_reference(type_ctx, unified_element_type);

    return true;
}

static bool zenit_type_ctx_unify_struct(struct ZenitTypeContext *type_ctx, struct ZenitStructType *struct_type, struct ZenitType *type_b, struct ZenitType **dest)
{
    if (struct_type == NULL || type_b == NULL)
        return false;

    if (type_b->typekind == ZENIT_TYPE_NONE)
    {
        *dest = (struct ZenitType*) zenit_type_ctx_copy_struct(type_ctx, struct_type);
        return true;
    }

    if (type_b->typekind != ZENIT_TYPE_STRUCT)
        return false;

    if (zenit_type_struct_equals(struct_type, type_b))
    {
        *dest = (struct ZenitType*) zenit_type_ctx_copy_struct(type_ctx, struct_type);
        return true;
    }

    struct ZenitStructType *struct_type_b = (struct ZenitStructType*) type_b;

    // If they are structurally equals, we just copy one of them and return an unnamed struct
    // (an unnamed struct can be casted or assigned to a named struct as long as they are structurally
    // equals and the other way around)
    bool struct_equals = zenit_type_struct_structurally_equals(struct_type, struct_type_b);

    struct ZenitStructType *unified_struct = zenit_type_ctx_new_struct(type_ctx, NULL);

    struct FlListNode *struct_a_node = fl_list_head(struct_type->members);
    while (struct_a_node)
    {
        struct ZenitStructTypeMember *struct_a_member = (struct ZenitStructTypeMember*) struct_a_node->value;

        struct ZenitStructTypeMember *struct_b_member = struct_equals ? NULL : zenit_type_struct_get_member(struct_type_b, struct_a_member->name);

        // If the structs are structurally equals, we can safely add the member, but if not, we need to make sure
        // the member is present in both types
        if (struct_equals || struct_b_member != NULL)
        {
            struct ZenitType *member_type = NULL;
            if (struct_equals)
            {
                // Equals means there is no need to unify the member type
                member_type = zenit_type_ctx_copy_type(type_ctx, struct_a_member->type);
            }
            else
            {
                if (!zenit_type_can_unify(struct_a_member->type, struct_b_member->type)
                    || !zenit_type_ctx_unify_types(type_ctx, struct_a_member->type, struct_b_member->type, &member_type))
                    return false;
            }
            
            zenit_type_struct_add_member(unified_struct, struct_a_member->name, member_type);
        }

        struct_a_node = struct_a_node->next;
    }

    *dest = (struct ZenitType*) unified_struct;
    return true;
}

static bool zenit_type_ctx_unify_uint(struct ZenitTypeContext *type_ctx, struct ZenitUintType *uint_type, struct ZenitType *type_b, struct ZenitType **dest)
{
    if (uint_type == NULL || type_b == NULL)
        return false;

    if (type_b->typekind != ZENIT_TYPE_NONE && type_b->typekind != ZENIT_TYPE_UINT)
        return false;

    if (type_b->typekind == ZENIT_TYPE_NONE || zenit_type_uint_equals(uint_type, type_b))
    {
        *dest = (struct ZenitType*) zenit_type_ctx_copy_uint(type_ctx, uint_type);
        return true;
    }

    // At this point, type_b must be a uint
    struct ZenitUintType *uint_b = (struct ZenitUintType*) type_b;
    *dest = (struct ZenitType*) zenit_type_ctx_copy_uint(type_ctx, uint_type->size > uint_b->size ? uint_type : uint_b);
    return true;
}

static bool zenit_type_ctx_unify_bool(struct ZenitTypeContext *type_ctx, struct ZenitBoolType *bool_type, struct ZenitType *type_b, struct ZenitType **dest)
{
    if (bool_type == NULL || type_b == NULL)
        return false;

    if (type_b->typekind != ZENIT_TYPE_NONE && type_b->typekind != ZENIT_TYPE_BOOL)
        return false;

    *dest = (struct ZenitType*) zenit_type_ctx_copy_bool(type_ctx, bool_type);
    return true;
}

bool zenit_type_ctx_unify_types(struct ZenitTypeContext *type_ctx, struct ZenitType *type_a, struct ZenitType *type_b, struct ZenitType **dest)
{
    if (type_a == NULL || type_b == NULL)
        return false;

    if (type_a->typekind == ZENIT_TYPE_NONE)
    {
        if (type_b->typekind == ZENIT_TYPE_NONE)
            return false;

        *dest = zenit_type_ctx_copy_type(type_ctx, type_b);
        return true;
    }

    if (type_a->typekind == ZENIT_TYPE_STRUCT)
        return zenit_type_ctx_unify_struct(type_ctx, (struct ZenitStructType*) type_a, type_b, dest);
    
    if (type_a->typekind == ZENIT_TYPE_REFERENCE)
        return zenit_type_ctx_unify_reference(type_ctx, (struct ZenitReferenceType*) type_a, type_b, dest);
    
    if (type_a->typekind == ZENIT_TYPE_ARRAY)
        return zenit_type_ctx_unify_array(type_ctx, (struct ZenitArrayType*) type_a, type_b, dest);
    
    if (type_a->typekind == ZENIT_TYPE_UINT)
        return zenit_type_ctx_unify_uint(type_ctx, (struct ZenitUintType*) type_a, type_b, dest);

    if (type_a->typekind == ZENIT_TYPE_BOOL)
        return zenit_type_ctx_unify_bool(type_ctx, (struct ZenitBoolType*) type_a, type_b, dest);
    
    return false;
}

void zenit_type_ctx_free(struct ZenitTypeContext *type_ctx)
{
    fl_list_free(type_ctx->uniques);
    fl_hashtable_free(type_ctx->pool);
    fl_free(type_ctx);
}
