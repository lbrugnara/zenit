
#include <fllib/IO.h>
#include "rom.h"

Rp2a03Rom* rp2a03_rom_new(Rp2a03Program *program)
{
    //size_t data_size = fl_array_length(program->data->bytes);
    //size_t startup_size = fl_array_length(program->startup->bytes);
    //size_t code_size = fl_array_length(program->code.bytes);
    //size_t total_size = data_size + startup_size + code_size;
//
    //// wrap
    //if (total_size < data_size || total_size < startup_size || total_size < code_size)
    //    return NULL;
//
    //if (total_size > sizeof(Rp2a03Nrom256))
    //    return NULL;    

    Rp2a03Rom default_rom = {
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
        .prg_rom = (Rp2a03Nrom256) {
            .bank = { 0 },
            .nmi_addr = 0,
            .res_addr = 0,
            .irq_addr = 0,
        }
    };

    // First we copy the data segment that actually uses the whole PRG-ROM (on purpose)
    memcpy(&default_rom.prg_rom, program->data->bytes, sizeof(Rp2a03Nrom256));

    // We need to copy the startup routine, we need to find the space for it
    if (program->startup->pc > 0)
    {
        // The startup routine will replace the original RESET vector, so we need to add a JMP at the end
        rp2a03_program_emit_abs(program, program->startup, NES_OP_JMP, default_rom.prg_rom.res_addr);

        size_t i=0;
        size_t bank_length = sizeof(default_rom.prg_rom.bank) / sizeof(default_rom.prg_rom.bank[0]);
        for (; i + program->startup->pc < bank_length; i++)
        {
            if (default_rom.prg_rom.bank[i] != 0 || default_rom.prg_rom.bank[i + program->startup->pc] != 0)
                continue;

            size_t j=i;
            for (; j < program->startup->pc + i; j++)
            {
                if (default_rom.prg_rom.bank[j] != 0)
                    break;
            }

            if (j == program->startup->pc + i)
            {
                memcpy(default_rom.prg_rom.bank + i, program->startup->bytes, program->startup->pc);
                default_rom.prg_rom.res_addr = program->data->base_address + i;
                break;
            }
        }

        if (i == bank_length - 1)
            return NULL;
    }

    // FIXME: Copy the code segment

    Rp2a03Rom *rom = fl_malloc(sizeof(Rp2a03Rom));
    memcpy(rom, &default_rom, sizeof(Rp2a03Rom));

    return rom;
}

void rp2a03_rom_free(Rp2a03Rom *rom)
{
    if (!rom)
        return;

    fl_free(rom);
}

void rp2a03_rom_dump(Rp2a03Rom *rom, const char *filename)
{
    FILE *file = fl_io_file_open(filename, "w+b");
    fl_io_file_write_bytes(file, sizeof(Rp2a03RomHeader), (const FlByte*)&rom->header);
    fl_io_file_write_bytes(file, sizeof(Rp2a03Nrom256), (const FlByte*)&rom->prg_rom);
    fl_io_file_close(file);
}
