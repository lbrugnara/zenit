#include "pool.h"

static unsigned long zir_type_pool_hash(const FlByte *key)
{
    if (!key)
        return 0;

    return zir_type_hash((struct ZirTypeInfo*) key);
}

struct ZirTypePool zir_type_pool_new(void)
{
    return (struct ZirTypePool) {
        .map = fl_hashtable_new_args((struct FlHashtableArgs) {
            .hash_function = zir_type_pool_hash,
            .key_allocator = NULL,
            .key_comparer = fl_container_equals_pointer,
            .key_cleaner = NULL,
            .value_cleaner = (FlContainerCleanupFunction) zir_type_free,
            .value_allocator = NULL
        })
    };
}

struct ZirTypeInfo* zir_type_pool_register(struct ZirTypePool *pool, struct ZirTypeInfo *typeinfo)
{
    if (typeinfo == NULL)
        return NULL;


    if (typeinfo->type == ZIR_TYPE_NONE)
    {
        //char *key = zir_type_to_string(typeinfo);

        if (!fl_hashtable_has_key(pool->map, typeinfo))
            typeinfo = fl_hashtable_add(pool->map, typeinfo, typeinfo);

        return typeinfo;
    }

    if (typeinfo->type == ZIR_TYPE_ARRAY)
        return (struct ZirTypeInfo*) zir_type_pool_register_array(pool, (struct ZirArrayTypeInfo*) typeinfo);

    if (typeinfo->type == ZIR_TYPE_REFERENCE)
        return (struct ZirTypeInfo*) zir_type_pool_register_reference(pool, (struct ZirReferenceTypeInfo*) typeinfo);

    if (typeinfo->type == ZIR_TYPE_STRUCT)
        return (struct ZirTypeInfo*) zir_type_pool_register_struct(pool, (struct ZirStructTypeInfo*) typeinfo);

    if (typeinfo->type == ZIR_TYPE_UINT)
        return (struct ZirTypeInfo*) zir_type_pool_register_primitive(pool, (struct ZirUintTypeInfo*) typeinfo);

    return NULL;
}

void zir_type_pool_free(struct ZirTypePool *pool)
{
    fl_hashtable_free(pool->map);
}

struct ZirArrayTypeInfo* zir_type_pool_register_array(struct ZirTypePool *pool, struct ZirArrayTypeInfo *typeinfo)
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

    zir_type_pool_register(pool, typeinfo->member_type);
    for (size_t i=0; i < fl_array_length(typeinfo->members); i++)
        zir_type_pool_register(pool, typeinfo->members[i]);

    return typeinfo;
}
    
struct ZirUintTypeInfo* zir_type_pool_register_primitive(struct ZirTypePool *pool, struct ZirUintTypeInfo *typeinfo)
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

struct ZirReferenceTypeInfo* zir_type_pool_register_reference(struct ZirTypePool *pool, struct ZirReferenceTypeInfo *typeinfo)
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

    zir_type_pool_register(pool, typeinfo->element);

    return typeinfo;
}

struct ZirStructTypeInfo* zir_type_pool_register_struct(struct ZirTypePool *pool, struct ZirStructTypeInfo *typeinfo)
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
