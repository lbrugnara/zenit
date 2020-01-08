#ifndef ZENIT_NES_EMIT
#define ZENIT_NES_EMIT

#include <stdlib.h>
#include "program.h"
#include "../../zir/instructions/operands/array.h"
#include "../../zir/instructions/operands/uint.h"
#include "../../zir/instructions/operands/reference.h"
#include "../../zir/instructions/operands/symbol.h"

void zenit_nes_emit_store_array(struct ZenitNesProgram *program, struct ZenitNesSymbol *nes_symbol, size_t offset, struct ZirArrayOperand *array);
void zenit_nes_emit_store_uint(struct ZenitNesProgram *program, struct ZenitNesSymbol *nes_symbol, size_t offset, struct ZirUintOperand *uint);
void zenit_nes_emit_store_reference(struct ZenitNesProgram *program, struct ZenitNesSymbol *nes_symbol, size_t offset, struct ZirReferenceOperand *uint_operand);
void zenit_nes_emit_store_symbol(struct ZenitNesProgram *program, struct ZenitNesSymbol *nes_symbol, size_t offset, struct ZirSymbolOperand *symbol_operand);

#endif /* ZENIT_NES_EMIT */
