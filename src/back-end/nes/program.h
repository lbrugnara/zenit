#ifndef ZENIT_NES_PROGRAM_H
#define ZENIT_NES_PROGRAM_H

#include <stdint.h>

#include "opcode.h"
#include "segment.h"
#include "symbols/symbol.h"

#include "../../zir/block.h"
#include "../../zir/symbol.h"
#include "../../zir/instructions/attributes/attribute.h"

typedef struct ZnesProgram {
    FlHashtable *symbols;
    ZnesZeroPageSegment zp;
    ZnesDataSegment data;
    ZnesCodeSegment startup;
    ZnesCodeSegment code;
    bool static_context;
} ZnesProgram;

ZnesProgram* zenit_nes_program_new(void);
void zenit_nes_program_free(ZnesProgram *program);
ZnesSymbol* zenit_nes_program_reserve_symbol(ZnesProgram *program, ZirBlock *block, ZirAttributeMap *attributes, ZirSymbol *zir_symbol);
ZnesSymbol* zenit_nes_program_get_tmpsym_symbol(ZnesProgram *program, ZnesSymbol *temp_symbol);

//uint16_t zenit_nes_program_emit_label(ZnesProgram *program);
//uint8_t zenit_nes_program_calc_rel_addr(ZnesProgram *program, uint16_t address);
void zenit_nes_program_emit_abs(ZnesCodeSegment *code, ZnesOpcode opcode, uint16_t bytes);
void zenit_nes_program_emit_abx(ZnesCodeSegment *code, ZnesOpcode opcode, uint16_t bytes);
void zenit_nes_program_emit_aby(ZnesCodeSegment *code, ZnesOpcode opcode, uint16_t bytes);
void zenit_nes_program_emit_imm(ZnesCodeSegment *code, ZnesOpcode opcode, uint8_t byte);
void zenit_nes_program_emit_imp(ZnesCodeSegment *code, ZnesOpcode opcode);
void zenit_nes_program_emit_ind(ZnesCodeSegment *code, ZnesOpcode opcode, uint16_t bytes);
void zenit_nes_program_emit_inx(ZnesCodeSegment *code, ZnesOpcode opcode, uint8_t byte);
void zenit_nes_program_emit_iny(ZnesCodeSegment *code, ZnesOpcode opcode, uint8_t byte);
void zenit_nes_program_emit_rel(ZnesCodeSegment *code, ZnesOpcode opcode, uint8_t byte);
void zenit_nes_program_emit_zpg(ZnesCodeSegment *code, ZnesOpcode opcode, uint8_t byte);
void zenit_nes_program_emit_zpx(ZnesCodeSegment *code, ZnesOpcode opcode, uint8_t byte);
void zenit_nes_program_emit_zpy(ZnesCodeSegment *code, ZnesOpcode opcode, uint8_t byte);

#endif /* ZENIT_NES_PROGRAM_H */
