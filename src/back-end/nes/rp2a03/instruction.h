#ifndef ZNES_INSTR_H
#define ZNES_INSTR_H

#include <stdint.h>
#include "mnemonic.h"

typedef struct Rp2a03Instruction {
    Rp2a03Mnemonic mnemonic;
    Rp2a03AddressMode mode;
    uint8_t size;
    const char *format;
} Rp2a03Instruction;

Rp2a03Instruction* rp2a03_instruction_lookup(uint8_t opcode);

#endif /* ZNES_INSTR_H */
