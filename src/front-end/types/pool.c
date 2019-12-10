#include "pool.h"

static unsigned long zenit_type_pool_hash(const FlByte *key)
{
    if (!key)
        return 0;

    return zenit_type_hash((struct ZenitTypeInfo*) key);
}

struct ZenitTypePool zenit_type_pool_new(void)
{
    return (struct ZenitTypePool) {
        .map = fl_hashtable_new_args((struct FlHashtableArgs) {
            .hash_function = zenit_type_pool_hash,
            //.key_allocator = fl_container_allocator_string,
            //.key_comparer = fl_container_equals_string,
            //.key_cleaner = fl_container_cleaner_pointer,
            .key_allocator = NULL,
            .key_comparer = fl_container_equals_pointer,
            .key_cleaner = NULL,
            .value_cleaner = (FlContainerCleanupFunction) zenit_type_free,
            .value_allocator = NULL
        })
    };
}

struct ZenitTypeInfo* zenit_type_pool_register(struct ZenitTypePool *pool, struct ZenitTypeInfo *typeinfo)
{
    if (typeinfo == NULL)
        return NULL;


    if (typeinfo->type == ZENIT_TYPE_NONE)
    {
        //char *key = zenit_type_to_string(typeinfo);

        if (!fl_hashtable_has_key(pool->map, typeinfo))
            typeinfo = fl_hashtable_add(pool->map, typeinfo, typeinfo);

        return typeinfo;
    }

    if (typeinfo->type == ZENIT_TYPE_ARRAY)
        return (struct ZenitTypeInfo*) zenit_type_pool_register_array(pool, (struct ZenitArrayTypeInfo*) typeinfo);

    if (typeinfo->type == ZENIT_TYPE_REFERENCE)
        return (struct ZenitTypeInfo*) zenit_type_pool_register_reference(pool, (struct ZenitReferenceTypeInfo*) typeinfo);

    if (typeinfo->type == ZENIT_TYPE_STRUCT)
        return (struct ZenitTypeInfo*) zenit_type_pool_register_struct(pool, (struct ZenitStructTypeInfo*) typeinfo);

    if (zenit_type_is_primitive(typeinfo->type))
        return (struct ZenitTypeInfo*) zenit_type_pool_register_primitive(pool, (struct ZenitPrimitiveTypeInfo*) typeinfo);

    return NULL;
}

void zenit_type_pool_free(struct ZenitTypePool *pool)
{
    fl_hashtable_free(pool->map);
}

struct ZenitArrayTypeInfo* zenit_type_pool_register_array(struct ZenitTypePool *pool, struct ZenitArrayTypeInfo *typeinfo)
{
    if (typeinfo == NULL)
        return NULL;

    if (!fl_hashtable_has_key(pool->map, typeinfo))
    {
        typeinfo = fl_hashtable_add(pool->map, typeinfo, typeinfo);

        // Error...
        if (typeinfo == NULL)
            return NULL;
    }    

    zenit_type_pool_register(pool, typeinfo->member_type);
    for (size_t i=0; i < fl_array_length(typeinfo->members); i++)
        zenit_type_pool_register(pool, typeinfo->members[i]);

    return typeinfo;
}
    
struct ZenitPrimitiveTypeInfo* zenit_type_pool_register_primitive(struct ZenitTypePool *pool, struct ZenitPrimitiveTypeInfo *typeinfo)
{
    if (typeinfo == NULL)
        return NULL;

    if (!fl_hashtable_has_key(pool->map, typeinfo))
    {
        typeinfo = fl_hashtable_add(pool->map, typeinfo, typeinfo);

        // Error...
        if (typeinfo == NULL)
            return NULL;
    }

    return typeinfo;
}

struct ZenitReferenceTypeInfo* zenit_type_pool_register_reference(struct ZenitTypePool *pool, struct ZenitReferenceTypeInfo *typeinfo)
{
    if (typeinfo == NULL)
        return NULL;

    if (!fl_hashtable_has_key(pool->map, typeinfo))
    {
        typeinfo = fl_hashtable_add(pool->map, typeinfo, typeinfo);

        // Error...
        if (typeinfo == NULL)
            return NULL;
    }

    zenit_type_pool_register(pool, typeinfo->element);

    return typeinfo;
}

struct ZenitStructTypeInfo* zenit_type_pool_register_struct(struct ZenitTypePool *pool, struct ZenitStructTypeInfo *typeinfo)
{
    if (typeinfo == NULL)
        return NULL;

    if (!fl_hashtable_has_key(pool->map, typeinfo))
    {
        typeinfo = fl_hashtable_add(pool->map, typeinfo, typeinfo);

        // Error...
        if (typeinfo == NULL)
            return NULL;
    }

    // FIXME: Register struct's members

    return typeinfo;
}
