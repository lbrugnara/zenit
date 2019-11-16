#include "back-end/nes/rom.h"

void write_program(struct NesRom *rom)
{
    uint16_t reset = nes_rom_emit_label(rom);
    nes_rom_emit_imp(rom, NES_OP_SEI);         // disable IRQs
    nes_rom_emit_imp(rom, NES_OP_CLD);         // disable decimal mode
    nes_rom_emit_imm(rom, NES_OP_LDX, 0x40);
    nes_rom_emit_abs(rom, NES_OP_STX, 0x4017); // disable APU frame IRQ
    nes_rom_emit_imm(rom, NES_OP_LDX, 0xFF);
    nes_rom_emit_imp(rom, NES_OP_TXS);         // Set up stack
    nes_rom_emit_imp(rom, NES_OP_INX);         // now X = 0
    nes_rom_emit_abs(rom, NES_OP_STX, 0x2000); // disable NMI
    nes_rom_emit_abs(rom, NES_OP_STX, 0x2001); // disable rendering
    nes_rom_emit_abs(rom, NES_OP_STX, 0x4010); // disable DMC IRQs
        
    uint16_t vblankwait1 = nes_rom_emit_label(rom); // First wait for vblank to make sure PPU is ready
    nes_rom_emit_abs(rom, NES_OP_BIT, 0x2002);
    nes_rom_emit_rel(rom, NES_OP_BPL, nes_rom_calc_rel_addr(rom, vblankwait1));

    uint16_t clrmem = nes_rom_emit_label(rom);
    nes_rom_emit_imm(rom, NES_OP_LDA, 0x00);
    nes_rom_emit_abx(rom, NES_OP_STA, 0x0000);
    nes_rom_emit_abx(rom, NES_OP_STA, 0x0100);
    nes_rom_emit_abx(rom, NES_OP_STA, 0x0200);
    nes_rom_emit_abx(rom, NES_OP_STA, 0x0400);
    nes_rom_emit_abx(rom, NES_OP_STA, 0x0500);
    nes_rom_emit_abx(rom, NES_OP_STA, 0x0600);
    nes_rom_emit_abx(rom, NES_OP_STA, 0x0700);
    nes_rom_emit_imm(rom, NES_OP_LDA, 0xFE);
    nes_rom_emit_abx(rom, NES_OP_STA, 0x0300);
    nes_rom_emit_imp(rom, NES_OP_INX);
    nes_rom_emit_rel(rom, NES_OP_BNE, nes_rom_calc_rel_addr(rom, clrmem));

    uint16_t vblankwait2 = nes_rom_emit_label(rom); // Second wait for vblank, PPU is ready after this
    nes_rom_emit_abs(rom, NES_OP_BIT, 0x2002);
    nes_rom_emit_rel(rom, NES_OP_BPL, nes_rom_calc_rel_addr(rom, vblankwait2));
    
    nes_rom_emit_imm(rom, NES_OP_LDA, 0x40); // intensify blues (01000000)
    nes_rom_emit_abs(rom, NES_OP_STA, 0x2001);

    uint16_t forever = nes_rom_emit_label(rom);
    nes_rom_emit_abs(rom, NES_OP_JMP, forever); // jump back to Forever, infinite loop
  
    uint16_t nmi = nes_rom_emit_label(rom);
    nes_rom_emit_imp(rom, NES_OP_RTI);

    uint16_t irq = nes_rom_emit_label(rom);
    nes_rom_emit_imp(rom, NES_OP_RTI);

    rom->prg_rom.nmi_addr = nmi;
    rom->prg_rom.res_addr = reset;
    rom->prg_rom.irq_addr = irq;
}

int main(void)
{
    struct NesRom rom = nes_rom_new(0x8000);
    write_program(&rom);

    nes_rom_dump(&rom, "rom.nes");

    return 0;
}
