#include "front-end/type-check/check.h"
#include "front-end/inference/infer.h"
#include "front-end/parser/parse.h"
#include "front-end/binding/resolve.h"
#include "front-end/symtable.h"
#include "front-end/codegen/zir.h"
#include "back-end/nes/rp2a03/generate.h"
#include "back-end/nes/ir/generate.h"
#include "back-end/nes/rp2a03/rom.h"

int main(int argc, char **argv)
{
    if (argc < 3)
        return -1;

    ZenitContext zenit_context = zenit_context_new(ZENIT_SOURCE_FILE, argv[1]);

    if (!zenit_parse_source(&zenit_context)
        || !zenit_resolve_symbols(&zenit_context)
        || !zenit_infer_types(&zenit_context)
        || !zenit_check_types(&zenit_context))
    {
        zenit_context_print_errors(&zenit_context);
        return -2;
    }
    
    ZirProgram *zir_program = zenit_generate_zir(&zenit_context);

    if (!zir_program)
    {
        zenit_context_print_errors(&zenit_context);
        return -3;
    }

    ZnesContext *znes_context = znes_context_new(false);

    if (!znes_generate_program(znes_context, zir_program))
        return -4;

    
    Rp2a03Program *rp2a03_program = rp2a03_generate_program(znes_context->program);

    if (!rp2a03_program)
        return -4;

    Rp2a03Rom *rom = rp2a03_rom_new(rp2a03_program);

    if (!rom)
        return -5;

    rp2a03_rom_dump(rom, argv[2]);
    
    rp2a03_rom_free(rom);
    rp2a03_program_free(rp2a03_program);
    znes_context_free(znes_context);
    zir_program_free(zir_program);
    zenit_context_free(&zenit_context);
    return 0;
}
