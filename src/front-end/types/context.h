#ifndef ZENIT_TYPE_SYSTEM_H
#define ZENIT_TYPE_SYSTEM_H

#include <fllib.h>

#include "type.h"
#include "array.h"
#include "none.h"
#include "uint.h"
#include "reference.h"
#include "struct.h"

/*
 * Struct: struct ZenitTypeContext
 *  Contains information about the different types of the system that are created using
 *  the functions in this module
 * 
 * Members:
 *  <FlHashtable> *pool: Set of reusable types
 *  <FlList> *uniques: Set of types created by the system that can't be reused
 */
struct ZenitTypeContext {
    FlHashtable *pool;
    FlList *uniques;
};

/*
 * Function: zenit_type_ctx_new
 *  Creates a new typing context
 *
 * Parameters:
 *  This function does not take parameters
 *
 * Returns:
 *  struct ZenitTypeContext*: The type context object
 *
 * Notes:
 *  The object returned by this function must be freed using the
 *  <zenit_type_ctx_free> function
 */
struct ZenitTypeContext* zenit_type_ctx_new(void);

/*
 * Function: zenit_type_ctx_new_array
 *  Creates a new array type object
 *
 * Parameters:
 *  <struct ZenitTypeContext> *type_ctx: The type context object
 *  <struct ZenitType> *member_type:  The type for the array's members
 *
 * Returns:
 *  struct ZenitArrayType*: The new array type object
 *
 * Notes:
 *  The <struct ZenitTypeContext> object takes ownership of the created type, which means that the caller does
 *  not need to free the memory used by the type object.
 */
struct ZenitArrayType* zenit_type_ctx_new_array(struct ZenitTypeContext *type_ctx, struct ZenitType *member_type);

/*
 * Function: zenit_type_ctx_new_none
 *  Creates a new none type object
 *
 * Parameters:
 *  <struct ZenitTypeContext> *type_ctx: The type context object
 *
 * Returns:
 *  struct ZenitType*: The new none type obj
 *
 * Notes:
 *  The <struct ZenitTypeContext> object takes ownership of the created type, which means that the caller does
 *  not need to free the memory used by the type object.
 */
struct ZenitType* zenit_type_ctx_new_none(struct ZenitTypeContext *type_ctx);

/*
 * Function: zenit_type_ctx_new_reference
 *  Creates a new reference type object
 *
 * Parameters:
 *  <struct ZenitTypeContext> *type_ctx: The type context object
 *  <struct ZenitType> *element: Type of the referenced symbol
 *
 * Returns:
 *  struct ZenitReferenceType*: The new reference type object
 *
 * Notes:
 *  The <struct ZenitTypeContext> object takes ownership of the created type, which means that the caller does
 *  not need to free the memory used by the type object.
 */
struct ZenitReferenceType* zenit_type_ctx_new_reference(struct ZenitTypeContext *type_ctx, struct ZenitType *element);

/*
 * Function: zenit_type_ctx_new_struct
 *  Creates a new struct type object
 *
 * Parameters:
 *  <struct ZenitTypeContext> *type_ctx: The type context object
 *  <char> *name: A valid string for the name of the struct type or <NULL> for unnamed structs.
 *
 * Returns:
 *  struct ZenitStructType*: The new struct type object
 *
 * Notes:
 *  The <struct ZenitTypeContext> object takes ownership of the created type, which means that the caller does
 *  not need to free the memory used by the type object.
 */
struct ZenitStructType* zenit_type_ctx_new_struct(struct ZenitTypeContext *type_ctx, char *name);

/*
 * Function: zenit_type_ctx_get_named_struct
 *  Returns a named struct that must be already present in the type context
 *
 * Parameters:
 *  <struct ZenitTypeContext> *type_ctx: The type context object
 *  <char> *name: A valid string for the name of the struct type. It cannot be <NULL>
 *
 * Returns:
 *  struct ZenitStructType*: The named struct type or NULL if it doesn't exist
 */
struct ZenitStructType* zenit_type_ctx_get_named_struct(struct ZenitTypeContext *type_ctx, char *name);

/*
 * Function: zenit_type_ctx_new_uint
 *  Creates a new uint type object
 *
 * Parameters:
 *  <struct ZenitTypeContext> *type_ctx: The type context object
 *  <enum ZenitUintTypeSize> size: The size of the uint type
 *
 * Returns:
 *  struct ZenitUintType*: The new uint type object
 *
 * Notes:
 *  The <struct ZenitTypeContext> object takes ownership of the created type, which means that the caller does
 *  not need to free the memory used by the type object.
 */
struct ZenitUintType* zenit_type_ctx_new_uint(struct ZenitTypeContext *type_ctx, enum ZenitUintTypeSize size);

/*
 * Function: zenit_type_ctx_copy_type
 *  Creates a copy of the source type and returns that copy to the caller
 *
 * Parameters:
 *  <struct ZenitTypeContext> *type_ctx: Type context object
 *  <struct ZenitType> *src_type: The source type
 *
 * Returns:
 *  struct ZenitType*: The type object copied from the source type
 *
 * Notes:
 *  The <struct ZenitTypeContext> object takes ownership of the created type, which means that the caller does
 *  not need to free the memory used by the type object.
 */
struct ZenitType* zenit_type_ctx_copy_type(struct ZenitTypeContext *type_ctx, struct ZenitType *src_type);

/*
 * Function: zenit_type_ctx_copy_array
 *  Creates a copy of the source type and returns that copy to the caller
 *
 * Parameters:
 *  <struct ZenitTypeContext> *type_ctx: Type context object
 *  <struct ZenitArrayType> *src_array_type: The source type
 *
 * Returns:
 *  struct ZenitArrayType*: The type object copied from the source type
 *
 * Notes:
 *  The <struct ZenitTypeContext> object takes ownership of the created type, which means that the caller does
 *  not need to free the memory used by the type object.
 */
struct ZenitArrayType* zenit_type_ctx_copy_array(struct ZenitTypeContext *type_ctx, struct ZenitArrayType *src_array_type);

/*
 * Function: zenit_type_ctx_copy_reference
 *  Creates a copy of the source type and returns that copy to the caller
 *
 * Parameters:
 *  <struct ZenitTypeContext> *type_ctx: Type context object
 *  <struct ZenitReferenceType> *src_ref_type: The source type
 *
 * Returns:
 *  struct ZenitReferenceType*: The type object copied from the source type
 *
 * Notes:
 *  The <struct ZenitTypeContext> object takes ownership of the created type, which means that the caller does
 *  not need to free the memory used by the type object.
 */
struct ZenitReferenceType* zenit_type_ctx_copy_reference(struct ZenitTypeContext *type_ctx, struct ZenitReferenceType *src_ref_type);

/*
 * Function: zenit_type_ctx_copy_struct
 *  Creates a copy of the source type and returns that copy to the caller
 *
 * Parameters:
 *  <struct ZenitTypeContext> *type_ctx: Type context object
 *  <struct ZenitStructType> *src_struct_type: The source type
 *
 * Returns:
 *  struct ZenitStructType*: The type object copied from the source type
 *
 * Notes:
 *  The <struct ZenitTypeContext> object takes ownership of the created type, which means that the caller does
 *  not need to free the memory used by the type object.
 */
struct ZenitStructType* zenit_type_ctx_copy_struct(struct ZenitTypeContext *type_ctx, struct ZenitStructType *src_struct_type);

/*
 * Function: zenit_type_ctx_copy_uint
 *  Creates a copy of the source type and returns that copy to the caller
 *
 * Parameters:
 *  <struct ZenitTypeContext> *type_ctx: Type context object
 *  <struct ZenitUintType> *src_uint_type: The source type
 *
 * Returns:
 *  struct ZenitUintType*: The type object copied from the source type
 *
 * Notes:
 *  The <struct ZenitTypeContext> object takes ownership of the created type, which means that the caller does
 *  not need to free the memory used by the type object.
 */
struct ZenitUintType* zenit_type_ctx_copy_uint(struct ZenitTypeContext *type_ctx, struct ZenitUintType *src_uint_type);

/*
 * Function: zenit_type_ctx_unify_types
 *  Tries to unify *type_a* with *type_b* by searching a common ancestor between the 2 types. If the search succeed, the common ancestor
 *  type is placed in the pointer pointed by *dest*, and it returns *true*. If the unification process fails, the *dest* pointer remains
 *  untouched and the function returns *false*.
 *
 * Parameters:
 *  <struct ZenitTypeContext> *type_ctx: Type context object
 *  <struct ZenitType> *type_a: Type object
 *  <struct ZenitType> *type_b: Type object
 *  <struct ZenitType> **dest: Pointer to a type object where the unified type will be placed on success
 *
 * Returns:
 *  bool: *true* if the unification can be done, otherwise *false*.
 *
 * Notes:
 *  The <struct ZenitTypeContext> object takes ownership of the created type (*dest) if the unification process succeed, which means that
 *  the caller does not need to free the memory used by the *dest object when the function returns *true*.
 */
bool zenit_type_ctx_unify_types(struct ZenitTypeContext *type_ctx, struct ZenitType *type_a, struct ZenitType *type_b, struct ZenitType **dest);

/*
 * Function: zenit_type_ctx_free
 *  Frees the memory used by the type context object, including all the objects tracked by the context.
 *
 * Parameters:
 *  <struct ZenitTypeContext> *type_ctx: The type context object
 *
 * Returns:
 *  void: This function does not return a value
 *
 * Notes:
 *  This function frees the memory of its owned objects, which means the types created by this context cannot be used once the
 *  context is freed.
 */
void zenit_type_ctx_free(struct ZenitTypeContext *type_ctx);

#endif /* ZENIT_TYPE_SYSTEM_H */
