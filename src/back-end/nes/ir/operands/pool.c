#include "pool.h"
#include "operand.h"

ZnesOperandPool* znes_operand_pool_new(void)
{
    ZnesOperandPool *pool = fl_malloc(sizeof(ZnesOperandPool));
    pool->operands = fl_list_new_args((struct FlListArgs) {
        .value_cleaner = (FlContainerCleanupFn) znes_operand_free
    });

    return pool;
}

void znes_operand_pool_free(ZnesOperandPool *pool)
{
    fl_list_free(pool->operands);
    fl_free(pool);
}

void znes_operand_pool_register(ZnesOperandPool *pool, ZnesOperand *operand)
{
    fl_list_append(pool->operands, operand);
}
