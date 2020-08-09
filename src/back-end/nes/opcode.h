#ifndef ZENIT_NES_OPCODE_H
#define ZENIT_NES_OPCODE_H

#include <stdint.h>

typedef enum ZnesOpcode {
    NES_OP_ADC, NES_OP_AND, NES_OP_ASL, NES_OP_BCC, NES_OP_BCS, NES_OP_BEQ, NES_OP_BIT, NES_OP_BMI, NES_OP_BNE, NES_OP_BPL, NES_OP_BRK, NES_OP_BVC,
    NES_OP_BVS, NES_OP_CLC, NES_OP_CLD, NES_OP_CLI, NES_OP_CLV, NES_OP_CMP, NES_OP_CPX, NES_OP_CPY, NES_OP_DEC, NES_OP_DEX, NES_OP_DEY, NES_OP_EOR,
    NES_OP_INC, NES_OP_INX, NES_OP_INY, NES_OP_JMP, NES_OP_JSR, NES_OP_LDA, NES_OP_LDX, NES_OP_LDY, NES_OP_LSR, NES_OP_NOP, NES_OP_ORA, NES_OP_PHA,
    NES_OP_PHP, NES_OP_PLA, NES_OP_PLP, NES_OP_ROL, NES_OP_ROR, NES_OP_RTI, NES_OP_RTS, NES_OP_SBC, NES_OP_SEC, NES_OP_SED, NES_OP_SEI, NES_OP_STA,
    NES_OP_STX, NES_OP_STY, NES_OP_TAX, NES_OP_TAY, NES_OP_TSX, NES_OP_TXA, NES_OP_TXS, NES_OP_TYA, NES_OP_XXX
} ZnesOpcode;

typedef enum ZnesAddressMode {
    NES_ADDR_ABS,
    NES_ADDR_ABX,
    NES_ADDR_ABY,
    NES_ADDR_IMM,
    NES_ADDR_IMP,
    NES_ADDR_IND,
    NES_ADDR_INX,
    NES_ADDR_INY,
    NES_ADDR_REL,
    NES_ADDR_ZPG,
    NES_ADDR_ZPX,
    NES_ADDR_ZPY
} ZnesAddressMode;


typedef struct ZnesInstruction {
    ZnesOpcode opcode;
    ZnesAddressMode mode;
    const char *mnemonic;
} ZnesInstruction;

uint8_t zenit_nes_opcode_lookup(ZnesOpcode opcode, ZnesAddressMode mode);

#endif /* ZENIT_NES_OPCODE_H */
