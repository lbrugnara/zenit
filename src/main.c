#include "front-end/phases/check.h"
#include "front-end/inference/infer.h"
#include "front-end/phases/parse.h"
#include "front-end/phases/resolve.h"
#include "front-end/symtable.h"
#include "front-end/phases/zirgen.h"
#include "back-end/nes/rp2a03/generate.h"
#include "back-end/nes/ir/generate.h"
#include "back-end/nes/rp2a03/rom.h"

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

    ZnesProgram *znes_program = znes_generate_program(zir_program, false);

    if (!znes_program)
        return -4;

    
    Rp2a03Program *rp2a03_program = rp2a03_generate_program(znes_program);

    if (!rp2a03_program)
        return -4;

    Rp2a03Rom *rom = rp2a03_rom_new(rp2a03_program);

    if (!rom)
        return -5;

    rp2a03_rom_dump(rom, argv[2]);
    
    rp2a03_rom_free(rom);
    rp2a03_program_free(rp2a03_program);
    znes_program_free(znes_program);
    zir_program_free(zir_program);
    zenit_context_free(&ctx);
    return 0;
}
