#include "pool.h"
#include "operand.h"

struct ZirOperandPool* zir_operand_pool_new(void)
{
    struct ZirOperandPool *pool = fl_malloc(sizeof(struct ZirOperandPool));
    pool->operands = fl_list_new_args((struct FlListArgs) {
        .value_cleaner = (FlContainerCleanupFn) zir_operand_free
    });

    return pool;
}
void zir_operand_pool_free(struct ZirOperandPool *pool)
{
    fl_list_free(pool->operands);
    fl_free(pool);
}

struct ZirArrayOperand* zir_operand_pool_new_array(struct ZirOperandPool *pool, struct ZirArrayType *type)
{
    struct ZirArrayOperand *array_operand = zir_operand_array_new(type);
    fl_list_append(pool->operands, array_operand);
    return array_operand;
}

struct ZirStructOperand* zir_operand_pool_new_struct(struct ZirOperandPool *pool, struct ZirStructType *type)
{
    struct ZirStructOperand *struct_operand = zir_operand_struct_new(type);
    fl_list_append(pool->operands, struct_operand);
    return struct_operand;
}

struct ZirReferenceOperand* zir_operand_pool_new_reference(struct ZirOperandPool *pool, struct ZirReferenceType *type, struct ZirSymbolOperand *operand)
{
    struct ZirReferenceOperand *reference_operand = zir_operand_reference_new(type, operand);
    fl_list_append(pool->operands, reference_operand);
    return reference_operand;
}

struct ZirSymbolOperand* zir_operand_pool_new_symbol(struct ZirOperandPool *pool, struct ZirSymbol *symbol)
{
    struct ZirSymbolOperand *symbol_operand = zir_operand_symbol_new(symbol);
    fl_list_append(pool->operands, symbol_operand);
    return symbol_operand;
}

struct ZirUintOperand* zir_operand_pool_new_uint(struct ZirOperandPool *pool, struct ZirUintType *type, union ZirUintValue value)
{
    struct ZirUintOperand *uint_operand = zir_operand_uint_new(type, value);
    fl_list_append(pool->operands, uint_operand);
    return uint_operand;
}

struct ZirBoolOperand* zir_operand_pool_new_bool(struct ZirOperandPool *pool, struct ZirBoolType *type, bool value)
{
    struct ZirBoolOperand *bool_operand = zir_operand_bool_new(type, value);
    fl_list_append(pool->operands, bool_operand);
    return bool_operand;
}

