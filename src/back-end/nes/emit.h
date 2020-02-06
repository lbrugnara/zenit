#ifndef ZENIT_NES_EMIT
#define ZENIT_NES_EMIT

#include <stdlib.h>
#include "program.h"
#include "../../zir/instructions/operands/array.h"
#include "../../zir/instructions/operands/uint.h"
#include "../../zir/instructions/operands/reference.h"
#include "../../zir/instructions/operands/struct.h"
#include "../../zir/instructions/operands/symbol.h"

void zenit_nes_emit_store_array(struct ZenitNesProgram *program, struct ZirArrayOperand *array, struct ZenitNesSymbol *nes_symbol, size_t offset, size_t gap);
void zenit_nes_emit_store_struct(struct ZenitNesProgram *program, struct ZirStructOperand *struct_operand, struct ZenitNesSymbol *nes_symbol, size_t offset);
void zenit_nes_emit_store_uint(struct ZenitNesProgram *program, struct ZirUintOperand *uint_operand, struct ZenitNesSymbol *nes_symbol, size_t offset);
void zenit_nes_emit_store_reference(struct ZenitNesProgram *program, struct ZirReferenceOperand *uint_operand, struct ZenitNesSymbol *nes_symbol, size_t offset);
void zenit_nes_emit_store_symbol(struct ZenitNesProgram *program, struct ZirSymbolOperand *symbol_operand, struct ZenitNesSymbol *nes_symbol, size_t offset);

#endif /* ZENIT_NES_EMIT */
