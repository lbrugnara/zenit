#include <fllib.h>
#include "rom.h"

struct ZenitNesRom* zenit_nes_rom_new(struct ZenitNesProgram *program)
{
    //size_t data_size = fl_array_length(program->data.bytes);
    //size_t startup_size = fl_array_length(program->startup.bytes);
    //size_t code_size = fl_array_length(program->code.bytes);
    //size_t total_size = data_size + startup_size + code_size;
//
    //// wrap
    //if (total_size < data_size || total_size < startup_size || total_size < code_size)
    //    return NULL;
//
    //if (total_size > sizeof(struct ZenitNesNrom256))
    //    return NULL;    

    struct ZenitNesRom default_rom = {
        .header = {
            .magic = { 0x4E, 0x45, 0x53, 0x1A },
            .prg_rom = 2,
            .chr_rom = 1,
            .flag6 = 0,
            .flag7 = 0,
            .flag8 = 0,
            .flag9 = 0,
            .flag10 = 0,
            .unused = { 0 }
        },
        .prg_rom = (struct ZenitNesNrom256) {
            .bank = { 0 },
            .nmi_addr = 0,
            .res_addr = 0,
            .irq_addr = 0,
        }
    };

    // First we copy the data segment that actually uses the whole PRG-ROM (on purpose)
    memcpy(&default_rom.prg_rom, program->data.bytes, sizeof(struct ZenitNesNrom256));

    // We need to copy the startup routine, we need to find the space for it
    if (program->startup.pc > 0)
    {
        // The startup routine will replace the original RESET vector, so we need to add a JMP at the end
        zenit_nes_program_emit_abs(&program->startup, NES_OP_JMP, default_rom.prg_rom.res_addr);

        size_t i=0;
        size_t bank_length = sizeof(default_rom.prg_rom.bank) / sizeof(default_rom.prg_rom.bank[0]);
        for (; i + program->startup.pc < bank_length; i++)
        {
            if (default_rom.prg_rom.bank[i] != 0 || default_rom.prg_rom.bank[i + program->startup.pc] != 0)
                continue;

            size_t j=i;
            for (; j < program->startup.pc + i; j++)
            {
                if (default_rom.prg_rom.bank[j] != 0)
                    break;
            }

            if (j == program->startup.pc + i)
            {
                memcpy(default_rom.prg_rom.bank + i, program->startup.bytes, program->startup.pc);
                default_rom.prg_rom.res_addr = program->data.base_address + i;
                break;
            }
        }

        if (i == bank_length - 1)
            return NULL;
    }

    // FIXME: Copy the code segment

    struct ZenitNesRom *rom = fl_malloc(sizeof(struct ZenitNesRom));
    memcpy(rom, &default_rom, sizeof(struct ZenitNesRom));

    return rom;
}

void zenit_nes_rom_free(struct ZenitNesRom *rom)
{
    if (!rom)
        return;

    fl_free(rom);
}

void zenit_nes_rom_dump(struct ZenitNesRom *rom, const char *filename)
{
    FILE *file = fl_io_file_open(filename, "w+b");
    fl_io_file_write_bytes(file, sizeof(struct ZenitNesFileHeader), (const FlByte*)&rom->header);
    fl_io_file_write_bytes(file, sizeof(struct ZenitNesNrom256), (const FlByte*)&rom->prg_rom);
    fl_io_file_close(file);
}
