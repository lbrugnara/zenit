#include "pool.h"
#include "operand.h"

ZirOperandPool* zir_operand_pool_new(void)
{
    ZirOperandPool *pool = fl_malloc(sizeof(ZirOperandPool));
    pool->operands = fl_list_new_args((struct FlListArgs) {
        .value_cleaner = (FlContainerCleanupFn) zir_operand_free
    });

    return pool;
}
void zir_operand_pool_free(ZirOperandPool *pool)
{
    fl_list_free(pool->operands);
    fl_free(pool);
}

ZirArrayOperand* zir_operand_pool_new_array(ZirOperandPool *pool, ZirArrayType *type)
{
    ZirArrayOperand *array_operand = zir_operand_array_new(type);
    fl_list_append(pool->operands, array_operand);
    return array_operand;
}

ZirStructOperand* zir_operand_pool_new_struct(ZirOperandPool *pool, ZirStructType *type)
{
    ZirStructOperand *struct_operand = zir_operand_struct_new(type);
    fl_list_append(pool->operands, struct_operand);
    return struct_operand;
}

ZirReferenceOperand* zir_operand_pool_new_reference(ZirOperandPool *pool, ZirReferenceType *type, ZirSymbolOperand *operand)
{
    ZirReferenceOperand *reference_operand = zir_operand_reference_new(type, operand);
    fl_list_append(pool->operands, reference_operand);
    return reference_operand;
}

ZirSymbolOperand* zir_operand_pool_new_symbol(ZirOperandPool *pool, ZirSymbol *symbol)
{
    ZirSymbolOperand *symbol_operand = zir_operand_symbol_new(symbol);
    fl_list_append(pool->operands, symbol_operand);
    return symbol_operand;
}

ZirUintOperand* zir_operand_pool_new_uint(ZirOperandPool *pool, ZirUintType *type, ZirUintValue value)
{
    ZirUintOperand *uint_operand = zir_operand_uint_new(type, value);
    fl_list_append(pool->operands, uint_operand);
    return uint_operand;
}

ZirBoolOperand* zir_operand_pool_new_bool(ZirOperandPool *pool, ZirBoolType *type, bool value)
{
    ZirBoolOperand *bool_operand = zir_operand_bool_new(type, value);
    fl_list_append(pool->operands, bool_operand);
    return bool_operand;
}

