#ifndef ZENIT_TYPE_SYSTEM_H
#define ZENIT_TYPE_SYSTEM_H


#include <fllib/containers/List.h>
#include <fllib/containers/Hashtable.h>
#include "type.h"
#include "array.h"
#include "bool.h"
#include "none.h"
#include "uint.h"
#include "reference.h"
#include "struct.h"

typedef FlHashtable ZenitStringToTypeMap;
typedef FlList ZenitTypeList;

/*
 * Struct: ZenitTypeContext
 *  Contains information about the different types of the system that are created using
 *  the functions in this module
 * 
 * Members:
 *  <ZenitStringToTypeMap> *pool: Set of reusable types
 *  <ZenitTypeList> *uniques: Set of types created by the system that can't be reused
 */
typedef struct ZenitTypeContext {
    ZenitStringToTypeMap *pool;
    ZenitTypeList *uniques;
} ZenitTypeContext;

/*
 * Function: zenit_type_ctx_new
 *  Creates a new typing context
 *
 * Parameters:
 *  This function does not take parameters
 *
 * Returns:
 *  ZenitTypeContext*: The type context object
 *
 * Notes:
 *  The object returned by this function must be freed using the
 *  <zenit_type_ctx_free> function
 */
ZenitTypeContext* zenit_type_ctx_new(void);

/*
 * Function: zenit_type_ctx_new_array
 *  Creates a new array type object
 *
 * Parameters:
 *  <ZenitTypeContext> *type_ctx: The type context object
 *  <ZenitType> *member_type:  The type for the array's members
 *
 * Returns:
 *  ZenitArrayType*: The new array type object
 *
 * Notes:
 *  The <ZenitTypeContext> object takes ownership of the created type, which means that the caller does
 *  not need to free the memory used by the type object.
 */
ZenitArrayType* zenit_type_ctx_new_array(ZenitTypeContext *type_ctx, ZenitType *member_type);

/*
 * Function: zenit_type_ctx_new_none
 *  Creates a new none type object
 *
 * Parameters:
 *  <ZenitTypeContext> *type_ctx: The type context object
 *
 * Returns:
 *  ZenitType*: The new none type obj
 *
 * Notes:
 *  The <ZenitTypeContext> object takes ownership of the created type, which means that the caller does
 *  not need to free the memory used by the type object.
 */
ZenitType* zenit_type_ctx_new_none(ZenitTypeContext *type_ctx);

/*
 * Function: zenit_type_ctx_new_reference
 *  Creates a new reference type object
 *
 * Parameters:
 *  <ZenitTypeContext> *type_ctx: The type context object
 *  <ZenitType> *element: Type of the referenced symbol
 *
 * Returns:
 *  ZenitReferenceType*: The new reference type object
 *
 * Notes:
 *  The <ZenitTypeContext> object takes ownership of the created type, which means that the caller does
 *  not need to free the memory used by the type object.
 */
ZenitReferenceType* zenit_type_ctx_new_reference(ZenitTypeContext *type_ctx, ZenitType *element);

/*
 * Function: zenit_type_ctx_new_struct
 *  Creates a new struct type object
 *
 * Parameters:
 *  <ZenitTypeContext> *type_ctx: The type context object
 *  <char> *name: A valid string for the name of the struct type or <NULL> for unnamed structs.
 *
 * Returns:
 *  ZenitStructType*: The new struct type object
 *
 * Notes:
 *  The <ZenitTypeContext> object takes ownership of the created type, which means that the caller does
 *  not need to free the memory used by the type object.
 */
ZenitStructType* zenit_type_ctx_new_struct(ZenitTypeContext *type_ctx, char *name);

/*
 * Function: zenit_type_ctx_get_named_struct
 *  Returns a named struct that must be already present in the type context
 *
 * Parameters:
 *  <ZenitTypeContext> *type_ctx: The type context object
 *  <char> *name: A valid string for the name of the struct type. It cannot be <NULL>
 *
 * Returns:
 *  ZenitStructType*: The named struct type or NULL if it doesn't exist
 */
ZenitStructType* zenit_type_ctx_get_named_struct(ZenitTypeContext *type_ctx, char *name);

/*
 * Function: zenit_type_ctx_new_uint
 *  Creates a new uint type object
 *
 * Parameters:
 *  <ZenitTypeContext> *type_ctx: The type context object
 *  <ZenitUintTypeSize> size: The size of the uint type
 *
 * Returns:
 *  ZenitUintType*: The new uint type object
 *
 * Notes:
 *  The <ZenitTypeContext> object takes ownership of the created type, which means that the caller does
 *  not need to free the memory used by the type object.
 */
ZenitUintType* zenit_type_ctx_new_uint(ZenitTypeContext *type_ctx, ZenitUintTypeSize size);

/*
 * Function: zenit_type_ctx_new_bool
 *  Creates a new boolean type object
 *
 * Parameters:
 *  <ZenitTypeContext> *type_ctx: The type context object
 *
 * Returns:
 *  ZenitBoolType*: The new boolean type object
 *
 * Notes:
 *  The <ZenitTypeContext> object takes ownership of the created type, which means that the caller does
 *  not need to free the memory used by the type object.
 */
ZenitBoolType* zenit_type_ctx_new_bool(ZenitTypeContext *type_ctx);

/*
 * Function: zenit_type_ctx_copy_type
 *  Creates a copy of the source type and returns that copy to the caller
 *
 * Parameters:
 *  <ZenitTypeContext> *type_ctx: Type context object
 *  <ZenitType> *src_type: The source type
 *
 * Returns:
 *  ZenitType*: The type object copied from the source type
 *
 * Notes:
 *  The <ZenitTypeContext> object takes ownership of the created type, which means that the caller does
 *  not need to free the memory used by the type object.
 */
ZenitType* zenit_type_ctx_copy_type(ZenitTypeContext *type_ctx, ZenitType *src_type);

/*
 * Function: zenit_type_ctx_copy_array
 *  Creates a copy of the source type and returns that copy to the caller
 *
 * Parameters:
 *  <ZenitTypeContext> *type_ctx: Type context object
 *  <ZenitArrayType> *src_array_type: The source type
 *
 * Returns:
 *  ZenitArrayType*: The type object copied from the source type
 *
 * Notes:
 *  The <ZenitTypeContext> object takes ownership of the created type, which means that the caller does
 *  not need to free the memory used by the type object.
 */
ZenitArrayType* zenit_type_ctx_copy_array(ZenitTypeContext *type_ctx, ZenitArrayType *src_array_type);

/*
 * Function: zenit_type_ctx_copy_reference
 *  Creates a copy of the source type and returns that copy to the caller
 *
 * Parameters:
 *  <ZenitTypeContext> *type_ctx: Type context object
 *  <ZenitReferenceType> *src_ref_type: The source type
 *
 * Returns:
 *  ZenitReferenceType*: The type object copied from the source type
 *
 * Notes:
 *  The <ZenitTypeContext> object takes ownership of the created type, which means that the caller does
 *  not need to free the memory used by the type object.
 */
ZenitReferenceType* zenit_type_ctx_copy_reference(ZenitTypeContext *type_ctx, ZenitReferenceType *src_ref_type);

/*
 * Function: zenit_type_ctx_copy_struct
 *  Creates a copy of the source type and returns that copy to the caller
 *
 * Parameters:
 *  <ZenitTypeContext> *type_ctx: Type context object
 *  <ZenitStructType> *src_struct_type: The source type
 *
 * Returns:
 *  ZenitStructType*: The type object copied from the source type
 *
 * Notes:
 *  The <ZenitTypeContext> object takes ownership of the created type, which means that the caller does
 *  not need to free the memory used by the type object.
 */
ZenitStructType* zenit_type_ctx_copy_struct(ZenitTypeContext *type_ctx, ZenitStructType *src_struct_type);

/*
 * Function: zenit_type_ctx_copy_uint
 *  Creates a copy of the source type and returns that copy to the caller
 *
 * Parameters:
 *  <ZenitTypeContext> *type_ctx: Type context object
 *  <ZenitUintType> *src_uint_type: The source type
 *
 * Returns:
 *  ZenitUintType*: The type object copied from the source type
 *
 * Notes:
 *  The <ZenitTypeContext> object takes ownership of the created type, which means that the caller does
 *  not need to free the memory used by the type object.
 */
ZenitUintType* zenit_type_ctx_copy_uint(ZenitTypeContext *type_ctx, ZenitUintType *src_uint_type);

/*
 * Function: zenit_type_ctx_copy_bool
 *  Creates a copy of the source type and returns that copy to the caller
 *
 * Parameters:
 *  <ZenitTypeContext> *type_ctx: Type context object
 *  <ZenitBoolType> *src_bool_type: The source type
 *
 * Returns:
 *  ZenitBoolType*: The type object copied from the source type
 *
 * Notes:
 *  The <ZenitTypeContext> object takes ownership of the created type, which means that the caller does
 *  not need to free the memory used by the type object.
 */
ZenitBoolType* zenit_type_ctx_copy_bool(ZenitTypeContext *type_ctx, ZenitBoolType *src_bool_type);

/*
 * Function: zenit_type_ctx_unify_types
 *  Tries to unify *type_a* with *type_b* by searching a common ancestor between the 2 types. If the search succeed, the common ancestor
 *  type is placed in the pointer pointed by *dest*, and it returns *true*. If the unification process fails, the *dest* pointer remains
 *  untouched and the function returns *false*.
 *
 * Parameters:
 *  <ZenitTypeContext> *type_ctx: Type context object
 *  <ZenitType> *type_a: Type object
 *  <ZenitType> *type_b: Type object
 *  <ZenitType> **dest: Pointer to a type object where the unified type will be placed on success
 *
 * Returns:
 *  bool: *true* if the unification can be done, otherwise *false*.
 *
 * Notes:
 *  The <ZenitTypeContext> object takes ownership of the created type (*dest) if the unification process succeed, which means that
 *  the caller does not need to free the memory used by the *dest object when the function returns *true*.
 */
bool zenit_type_ctx_unify_types(ZenitTypeContext *type_ctx, ZenitType *type_a, ZenitType *type_b, ZenitType **dest);

/*
 * Function: zenit_type_ctx_free
 *  Frees the memory used by the type context object, including all the objects tracked by the context.
 *
 * Parameters:
 *  <ZenitTypeContext> *type_ctx: The type context object
 *
 * Returns:
 *  void: This function does not return a value
 *
 * Notes:
 *  This function frees the memory of its owned objects, which means the types created by this context cannot be used once the
 *  context is freed.
 */
void zenit_type_ctx_free(ZenitTypeContext *type_ctx);

#endif /* ZENIT_TYPE_SYSTEM_H */
