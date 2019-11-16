#include "nesrom.h"

void zenit_nes_symbol_free(struct ZenitNesSymbol *symbol)
{
    fl_free(symbol);
}

struct ZenitNesRom* zenit_nes_rom_new()
{
    struct ZenitNesRom *rom = fl_malloc(sizeof(struct ZenitNesRom));

    rom->text.content = fl_array_new(sizeof(uint8_t), 0);

    rom->zp = (struct ZenitNesRomZeroPageSection){
        .symbols = fl_hashtable_new_args((struct FlHashtableArgs) {
            .hash_function = fl_hashtable_hash_string,
            .key_allocator = fl_container_allocator_string,
            .key_comparer = fl_container_equals_string,
            .key_cleaner = fl_container_cleaner_pointer,
            .value_cleaner = (FlContainerCleanupFunction)zenit_nes_symbol_free,
            .value_allocator = NULL
        }),
        .slot = 0
    };

    rom->data = (struct ZenitNesRomDataSection){
        .symbols = fl_hashtable_new_args((struct FlHashtableArgs) {
            .hash_function = fl_hashtable_hash_string,
            .key_allocator = fl_container_allocator_string,
            .key_comparer = fl_container_equals_string,
            .key_cleaner = fl_container_cleaner_pointer,
            .value_cleaner = (FlContainerCleanupFunction)zenit_nes_symbol_free,
            .value_allocator = NULL
        }),
        .slot = 0
    };

    return rom;
}
