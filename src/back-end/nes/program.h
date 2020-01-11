#ifndef ZENIT_NES_PROGRAM_H
#define ZENIT_NES_PROGRAM_H

#include <stdint.h>
#include <fllib.h>
#include "opcode.h"

#include "../../zir/block.h"
#include "../../zir/symbol.h"
#include "../../zir/instructions/attributes/attribute.h"

enum ZenitNesSegment {
    ZENIT_NES_SEGMENT_ZP,
    ZENIT_NES_SEGMENT_DATA,
    ZENIT_NES_SEGMENT_CODE,
    ZENIT_NES_SEGMENT_TEMP,
};

struct ZenitNesSymbol {
    char *name;
    size_t elements;
    size_t size;
    size_t element_size;
    uint16_t address;
    enum ZenitNesSegment segment;
};

struct ZenitNesTmpSymbol {
    struct ZenitNesSymbol base;
    struct ZirOperand *source;
};

struct ZenitNesDataSegment {
    uint8_t *bytes;
    uint8_t *slots;
    uint16_t base_address;
};

struct ZenitNesCodeSegment {
    uint8_t *bytes;
    uint16_t pc;
};

struct ZenitNesZeroPageSegment {
    uint8_t slots[255];
};

struct ZenitNesProgram {
    FlHashtable symbols;
    struct ZenitNesZeroPageSegment zp;
    struct ZenitNesDataSegment data;
    struct ZenitNesCodeSegment startup;
    struct ZenitNesCodeSegment code;
    bool static_context;
};

struct ZenitNesProgram* zenit_nes_program_new(void);
void zenit_nes_program_free(struct ZenitNesProgram *program);
struct ZenitNesSymbol* zenit_nes_program_reserve_symbol(struct ZenitNesProgram *program, struct ZirBlock *block, struct ZirAttributeMap *attributes, struct ZirSymbol *zir_symbol);

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
