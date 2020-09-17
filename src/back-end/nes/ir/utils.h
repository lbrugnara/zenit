#ifndef ZNES_UTILS_H
#define ZNES_UTILS_H

#include "program.h"
#include "operands/operand.h"
#include "objects/alloc.h"
#include "../../../zir/block.h"

ZnesAllocType znes_alloc_type_from_zir_type(ZirType *type);
ZnesAllocType znes_utils_variable_type_from_operand(ZnesOperand *operand);
ZnesAllocType znes_utils_zir_type_to_nes_type(ZirSymbol *zir_symbol);
ZnesOperand* znes_utils_make_nes_operand(ZnesProgram *program, ZirOperand *zir_opn);
bool znes_alloc_request_init(ZnesProgram *program, ZirBlock *block, ZirSymbol *zir_symbol, ZirAttributeMap *attributes, ZnesAllocRequest *var_alloc);

#endif /* ZNES_UTILS_H */
