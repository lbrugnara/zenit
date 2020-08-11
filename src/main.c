#include "front-end/phases/check.h"
#include "front-end/phases/infer.h"
#include "front-end/phases/parse.h"
#include "front-end/phases/resolve.h"
#include "front-end/symtable.h"
#include "front-end/phases/zirgen.h"
#include "back-end/nes/program.h"
#include "back-end/nes/generate.h"
#include "back-end/nes/rom.h"

int main(int argc, char **argv)
{
    if (argc < 3)
        return -1;

    ZenitContext ctx = zenit_context_new(ZENIT_SOURCE_FILE, argv[1]);

    if (!zenit_parse_source(&ctx)
        || !zenit_resolve_symbols(&ctx)
        || !zenit_infer_types(&ctx)
        || !zenit_check_types(&ctx))
    {
        zenit_context_print_errors(&ctx);
        return -2;
    }
    
    ZirProgram *zir_program = zenit_generate_zir(&ctx);

    if (!zir_program)
    {
        zenit_context_print_errors(&ctx);
        return -3;
    }

    ZnesProgram *nes_program = znes_generate_program(zir_program);

    if (!nes_program)
        return -4;

    ZnesRom *rom = znes_rom_new(nes_program);

    if (!rom)
        return -5;

    znes_rom_dump(rom, argv[2]);
    
    znes_rom_free(rom);
    znes_program_free(nes_program);
    zir_program_free(zir_program);
    zenit_context_free(&ctx);
    return 0;
}
