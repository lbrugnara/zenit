#ifndef ZENIT_NES_PROGRAM_H
#define ZENIT_NES_PROGRAM_H

#include <stdint.h>

#include "opcode.h"
#include "segment.h"
#include "symbols/symbol.h"

#include "../../zir/block.h"
#include "../../zir/symbol.h"
#include "../../zir/instructions/attributes/attribute.h"

struct ZenitNesProgram {
    FlHashtable *symbols;
    struct ZenitNesZeroPageSegment zp;
    struct ZenitNesDataSegment data;
    struct ZenitNesCodeSegment startup;
    struct ZenitNesCodeSegment code;
    bool static_context;
};

struct ZenitNesProgram* zenit_nes_program_new(void);
void zenit_nes_program_free(struct ZenitNesProgram *program);
struct ZenitNesSymbol* zenit_nes_program_reserve_symbol(struct ZenitNesProgram *program, struct ZirBlock *block, ZirAttributeMap *attributes, struct ZirSymbol *zir_symbol);
struct ZenitNesSymbol* zenit_nes_program_get_tmpsym_symbol(struct ZenitNesProgram *program, struct ZenitNesSymbol *temp_symbol);

//uint16_t zenit_nes_program_emit_label(struct ZenitNesProgram *program);
//uint8_t zenit_nes_program_calc_rel_addr(struct ZenitNesProgram *program, uint16_t address);
void zenit_nes_program_emit_abs(struct ZenitNesCodeSegment *code, enum NesOpcode opcode, uint16_t bytes);
void zenit_nes_program_emit_abx(struct ZenitNesCodeSegment *code, enum NesOpcode opcode, uint16_t bytes);
void zenit_nes_program_emit_aby(struct ZenitNesCodeSegment *code, enum NesOpcode opcode, uint16_t bytes);
void zenit_nes_program_emit_imm(struct ZenitNesCodeSegment *code, enum NesOpcode opcode, uint8_t byte);
void zenit_nes_program_emit_imp(struct ZenitNesCodeSegment *code, enum NesOpcode opcode);
void zenit_nes_program_emit_ind(struct ZenitNesCodeSegment *code, enum NesOpcode opcode, uint16_t bytes);
void zenit_nes_program_emit_inx(struct ZenitNesCodeSegment *code, enum NesOpcode opcode, uint8_t byte);
void zenit_nes_program_emit_iny(struct ZenitNesCodeSegment *code, enum NesOpcode opcode, uint8_t byte);
void zenit_nes_program_emit_rel(struct ZenitNesCodeSegment *code, enum NesOpcode opcode, uint8_t byte);
void zenit_nes_program_emit_zpg(struct ZenitNesCodeSegment *code, enum NesOpcode opcode, uint8_t byte);
void zenit_nes_program_emit_zpx(struct ZenitNesCodeSegment *code, enum NesOpcode opcode, uint8_t byte);
void zenit_nes_program_emit_zpy(struct ZenitNesCodeSegment *code, enum NesOpcode opcode, uint8_t byte);

#endif /* ZENIT_NES_PROGRAM_H */
