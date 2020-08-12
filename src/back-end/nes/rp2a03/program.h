#ifndef RP2A03_PROGRAM_H
#define RP2A03_PROGRAM_H

#include <stdint.h>
#include <stdbool.h>
#include "segment-data.h"
#include "segment-text.h"
#include "mnemonic.h"

typedef struct Rp2a03Program {
    Rp2a03DataSegment *data;
    Rp2a03TextSegment *startup;
    Rp2a03TextSegment *code;
} Rp2a03Program;

Rp2a03Program* rp2a03_program_new(size_t data_base_address, size_t startup_base_address, size_t code_base_address);
void rp2a03_program_free(Rp2a03Program *program);
char* rp2a03_program_disassemble(Rp2a03Program *program);
void rp2a03_program_emit_abs(Rp2a03Program *program, Rp2a03TextSegment *segment, Rp2a03Mnemonic mnemonic, uint16_t bytes);
void rp2a03_program_emit_abx(Rp2a03Program *program, Rp2a03TextSegment *segment, Rp2a03Mnemonic mnemonic, uint16_t bytes);
void rp2a03_program_emit_aby(Rp2a03Program *program, Rp2a03TextSegment *segment, Rp2a03Mnemonic mnemonic, uint16_t bytes);
void rp2a03_program_emit_imm(Rp2a03Program *program, Rp2a03TextSegment *segment, Rp2a03Mnemonic mnemonic, uint8_t byte);
void rp2a03_program_emit_imp(Rp2a03Program *program, Rp2a03TextSegment *segment, Rp2a03Mnemonic mnemonic);
void rp2a03_program_emit_ind(Rp2a03Program *program, Rp2a03TextSegment *segment, Rp2a03Mnemonic mnemonic, uint16_t bytes);
void rp2a03_program_emit_inx(Rp2a03Program *program, Rp2a03TextSegment *segment, Rp2a03Mnemonic mnemonic, uint8_t byte);
void rp2a03_program_emit_iny(Rp2a03Program *program, Rp2a03TextSegment *segment, Rp2a03Mnemonic mnemonic, uint8_t byte);
void rp2a03_program_emit_rel(Rp2a03Program *program, Rp2a03TextSegment *segment, Rp2a03Mnemonic mnemonic, uint8_t byte);
void rp2a03_program_emit_zpg(Rp2a03Program *program, Rp2a03TextSegment *segment, Rp2a03Mnemonic mnemonic, uint8_t byte);
void rp2a03_program_emit_zpx(Rp2a03Program *program, Rp2a03TextSegment *segment, Rp2a03Mnemonic mnemonic, uint8_t byte);
void rp2a03_program_emit_zpy(Rp2a03Program *program, Rp2a03TextSegment *segment, Rp2a03Mnemonic mnemonic, uint8_t byte);

#endif /* RP2A03_PROGRAM_H */
