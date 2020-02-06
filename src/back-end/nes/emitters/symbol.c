#include "symbol.h"

#include "array.h"
#include "reference.h"
#include "struct.h"
#include "temp.h"
#include "uint.h"

#include "../symbols/array.h"
#include "../symbols/reference.h"
#include "../symbols/struct.h"
#include "../symbols/temp.h"
#include "../symbols/uint.h"

static void emit_zp_to_zp(struct ZenitNesProgram *program, struct ZenitNesSymbol *source_symbol, struct ZenitNesSymbol *nes_symbol, size_t offset)
{
    if (source_symbol->symkind != nes_symbol->symkind)
        return;    

    if (source_symbol->symkind == ZENIT_NES_SYMBOL_ARRAY)
    {
        struct ZenitNesArraySymbol *src_symbol = (struct ZenitNesArraySymbol*) source_symbol;
        struct ZenitNesArraySymbol *dst_symbol = (struct ZenitNesArraySymbol*) nes_symbol;

        for (size_t i=0; i < fl_array_length(dst_symbol->elements); i++)
        {
            struct ZenitNesSymbol *src_elem = src_symbol->elements[i];
            struct ZenitNesSymbol *dst_elem = dst_symbol->elements[i];

            emit_zp_to_zp(program, src_elem, dst_elem, 0);
        }
    }
    else if (source_symbol->symkind == ZENIT_NES_SYMBOL_STRUCT)
    {
        struct ZenitNesStructSymbol *src_symbol = (struct ZenitNesStructSymbol*) source_symbol;
        struct ZenitNesStructSymbol *dst_symbol = (struct ZenitNesStructSymbol*) nes_symbol;

        for (size_t i=0; i < fl_array_length(dst_symbol->members); i++)
        {
            struct ZenitNesSymbol *src_elem = src_symbol->members[i];
            struct ZenitNesSymbol *dst_elem = dst_symbol->members[i];

            emit_zp_to_zp(program, src_elem, dst_elem, 0);
        }
    }
    else
    {
        struct ZenitNesCodeSegment *target_segment = program->static_context ? &program->startup : &program->code;
        uint16_t target_address = nes_symbol->address + offset;

        // FIXME: fix this suboptimal implementation
        size_t to_copy = source_symbol->size > nes_symbol->size 
                ? nes_symbol->size 
                : source_symbol->size;

        for (size_t i=0; i < to_copy; i++)
        {
            zenit_nes_program_emit_zpg(target_segment, NES_OP_LDA, (uint8_t)(source_symbol->address + i));
            zenit_nes_program_emit_zpg(target_segment, NES_OP_STA, (uint8_t)(target_address + i));
        }

        if (nes_symbol->size > source_symbol->size)
        {
            zenit_nes_program_emit_imm(target_segment, NES_OP_LDA, 0x0);
            for (size_t i = to_copy; i < nes_symbol->size; i++)
                zenit_nes_program_emit_zpg(target_segment, NES_OP_STA, (uint8_t)(target_address + i));
        }
    }
}

static void emit_zp_to_code(struct ZenitNesProgram *program, struct ZenitNesSymbol *source_symbol, struct ZenitNesSymbol *nes_symbol, size_t offset)
{
    if (source_symbol->symkind != nes_symbol->symkind)
        return;

    if (source_symbol->symkind == ZENIT_NES_SYMBOL_ARRAY)
    {
        struct ZenitNesArraySymbol *src_symbol = (struct ZenitNesArraySymbol*) source_symbol;
        struct ZenitNesArraySymbol *dst_symbol = (struct ZenitNesArraySymbol*) nes_symbol;

        for (size_t i=0; i < fl_array_length(dst_symbol->elements); i++)
        {
            struct ZenitNesSymbol *src_elem = src_symbol->elements[i];
            struct ZenitNesSymbol *dst_elem = dst_symbol->elements[i];

            emit_zp_to_code(program, src_elem, dst_elem, 0);
        }
    }
    else if (source_symbol->symkind == ZENIT_NES_SYMBOL_STRUCT)
    {
        struct ZenitNesStructSymbol *src_symbol = (struct ZenitNesStructSymbol*) source_symbol;
        struct ZenitNesStructSymbol *dst_symbol = (struct ZenitNesStructSymbol*) nes_symbol;

        for (size_t i=0; i < fl_array_length(dst_symbol->members); i++)
        {
            struct ZenitNesSymbol *src_elem = src_symbol->members[i];
            struct ZenitNesSymbol *dst_elem = dst_symbol->members[i];

            emit_zp_to_code(program, src_elem, dst_elem, 0);
        }
    }
    else
    {
        struct ZenitNesCodeSegment *target_segment = program->static_context ? &program->startup : &program->code;
        uint16_t target_address = nes_symbol->address + offset;

        // FIXME: fix this suboptimal implementation
        size_t to_copy = source_symbol->size > nes_symbol->size 
                ? nes_symbol->size 
                : source_symbol->size;

        for (size_t i=0; i < to_copy; i++)
        {
            zenit_nes_program_emit_abs(target_segment, NES_OP_LDA, source_symbol->address + i);
            zenit_nes_program_emit_zpg(target_segment, NES_OP_STA, (uint8_t)(target_address + i));
        }

        if (nes_symbol->size > source_symbol->size)
        {
            zenit_nes_program_emit_imm(target_segment, NES_OP_LDA, 0x0);
            for (size_t i = to_copy; i < nes_symbol->size; i++)
                zenit_nes_program_emit_zpg(target_segment, NES_OP_STA, (uint8_t)(target_address + i));
        }
    }
}

static void emit_zp_to_data(struct ZenitNesProgram *program, struct ZenitNesSymbol *source_symbol, struct ZenitNesSymbol *nes_symbol, size_t offset)
{
    if (source_symbol->symkind != nes_symbol->symkind)
        return;

    if (source_symbol->symkind == ZENIT_NES_SYMBOL_ARRAY)
    {
        struct ZenitNesArraySymbol *src_symbol = (struct ZenitNesArraySymbol*) source_symbol;
        struct ZenitNesArraySymbol *dst_symbol = (struct ZenitNesArraySymbol*) nes_symbol;

        for (size_t i=0; i < fl_array_length(dst_symbol->elements); i++)
        {
            struct ZenitNesSymbol *src_elem = src_symbol->elements[i];
            struct ZenitNesSymbol *dst_elem = dst_symbol->elements[i];

            emit_zp_to_data(program, src_elem, dst_elem, 0);
        }
    }
    else if (source_symbol->symkind == ZENIT_NES_SYMBOL_STRUCT)
    {
        struct ZenitNesStructSymbol *src_symbol = (struct ZenitNesStructSymbol*) source_symbol;
        struct ZenitNesStructSymbol *dst_symbol = (struct ZenitNesStructSymbol*) nes_symbol;

        for (size_t i=0; i < fl_array_length(dst_symbol->members); i++)
        {
            struct ZenitNesSymbol *src_elem = src_symbol->members[i];
            struct ZenitNesSymbol *dst_elem = dst_symbol->members[i];

            emit_zp_to_data(program, src_elem, dst_elem, 0);
        }
    }
    else
    {
        struct ZenitNesCodeSegment *target_segment = program->static_context ? &program->startup : &program->code;
        uint16_t target_address = nes_symbol->address + offset;

        // FIXME: fix this suboptimal implementation
        size_t to_copy = source_symbol->size > nes_symbol->size 
                ? nes_symbol->size 
                : source_symbol->size;

        for (size_t i=0; i < to_copy; i++)
        {
            if (program->static_context)
            {
                // NOTE: In static context we can directly copy the value from the DATA segment
                zenit_nes_program_emit_imm(target_segment, NES_OP_LDA, program->data.bytes[source_symbol->address - program->data.base_address + i]);
            }
            else
            {
                // NOTE: If we are not in static context, we need to get the current -possibly modified- value from the DATA segment
                zenit_nes_program_emit_abs(target_segment, NES_OP_LDA, source_symbol->address + i);
            }

            zenit_nes_program_emit_zpg(target_segment, NES_OP_STA, (uint8_t)(target_address + i));
        }

        if (nes_symbol->size > source_symbol->size)
        {
            zenit_nes_program_emit_imm(target_segment, NES_OP_LDA, 0x0);
            for (size_t i = to_copy; i < nes_symbol->size; i++)
                zenit_nes_program_emit_zpg(target_segment, NES_OP_STA, (uint8_t)(target_address + i));
        }
    }
}

static void emit_data_to_data(struct ZenitNesProgram *program, struct ZenitNesSymbol *source_symbol, struct ZenitNesSymbol *nes_symbol, size_t offset)
{
    if (source_symbol->symkind != nes_symbol->symkind)
        return;

    if (source_symbol->symkind == ZENIT_NES_SYMBOL_ARRAY)
    {
        struct ZenitNesArraySymbol *src_symbol = (struct ZenitNesArraySymbol*) source_symbol;
        struct ZenitNesArraySymbol *dst_symbol = (struct ZenitNesArraySymbol*) nes_symbol;

        for (size_t i=0; i < fl_array_length(dst_symbol->elements); i++)
        {
            struct ZenitNesSymbol *src_elem = src_symbol->elements[i];
            struct ZenitNesSymbol *dst_elem = dst_symbol->elements[i];

            emit_data_to_data(program, src_elem, dst_elem, 0);
        }
    }
    else if (source_symbol->symkind == ZENIT_NES_SYMBOL_STRUCT)
    {
        struct ZenitNesStructSymbol *src_symbol = (struct ZenitNesStructSymbol*) source_symbol;
        struct ZenitNesStructSymbol *dst_symbol = (struct ZenitNesStructSymbol*) nes_symbol;

        for (size_t i=0; i < fl_array_length(dst_symbol->members); i++)
        {
            struct ZenitNesSymbol *src_elem = src_symbol->members[i];
            struct ZenitNesSymbol *dst_elem = dst_symbol->members[i];

            emit_data_to_data(program, src_elem, dst_elem, 0);
        }
    }
    else
    {
        uint16_t target_address = nes_symbol->address + offset;

        if (program->static_context)
        {
            // NOTE: In static context we can directly copy from one part of the DATA segment to another at compile time
            uint8_t *source = program->data.bytes + source_symbol->address - program->data.base_address;
            uint8_t *destination = program->data.bytes + (target_address - program->data.base_address);

            size_t to_copy = source_symbol->size > nes_symbol->size
                ? nes_symbol->size
                : source_symbol->size;

            memcpy(destination, source, to_copy);
            if (nes_symbol->size > source_symbol->size)
                memset(destination + to_copy, 0, nes_symbol->size - to_copy);
        }
        else
        {
            struct ZenitNesCodeSegment *target_segment = program->static_context ? &program->startup : &program->code;
            // NOTE: If we are not in static context, we need to copy from one part to the other of th DATA segment using
            // instructions
            // FIXME: fix this suboptimal implementation
            size_t to_copy = source_symbol->size > nes_symbol->size 
                    ? nes_symbol->size 
                    : source_symbol->size;

            for (size_t i=0; i < to_copy; i++)
            {
                zenit_nes_program_emit_abs(target_segment, NES_OP_LDA, source_symbol->address + i);
                zenit_nes_program_emit_abs(target_segment, NES_OP_STA, target_address + i);
            }

            if (nes_symbol->size > source_symbol->size)
            {
                zenit_nes_program_emit_imm(target_segment, NES_OP_LDA, 0x0);
                for (size_t i = to_copy; i < nes_symbol->size; i++)
                    zenit_nes_program_emit_abs(target_segment, NES_OP_STA, target_address + i);
            }
        }
    }
}

static void emit_data_to_zp(struct ZenitNesProgram *program, struct ZenitNesSymbol *source_symbol, struct ZenitNesSymbol *nes_symbol, size_t offset)
{
    if (source_symbol->symkind != nes_symbol->symkind)
        return;

    if (source_symbol->symkind == ZENIT_NES_SYMBOL_ARRAY)
    {
        struct ZenitNesArraySymbol *src_symbol = (struct ZenitNesArraySymbol*) source_symbol;
        struct ZenitNesArraySymbol *dst_symbol = (struct ZenitNesArraySymbol*) nes_symbol;

        for (size_t i=0; i < fl_array_length(dst_symbol->elements); i++)
        {
            struct ZenitNesSymbol *src_elem = src_symbol->elements[i];
            struct ZenitNesSymbol *dst_elem = dst_symbol->elements[i];

            emit_data_to_zp(program, src_elem, dst_elem, 0);
        }
    }
    else if (source_symbol->symkind == ZENIT_NES_SYMBOL_STRUCT)
    {
        struct ZenitNesStructSymbol *src_symbol = (struct ZenitNesStructSymbol*) source_symbol;
        struct ZenitNesStructSymbol *dst_symbol = (struct ZenitNesStructSymbol*) nes_symbol;

        for (size_t i=0; i < fl_array_length(dst_symbol->members); i++)
        {
            struct ZenitNesSymbol *src_elem = src_symbol->members[i];
            struct ZenitNesSymbol *dst_elem = dst_symbol->members[i];

            emit_data_to_zp(program, src_elem, dst_elem, 0);
        }
    }
    else
    {
        struct ZenitNesCodeSegment *target_segment = program->static_context ? &program->startup : &program->code;
        uint16_t target_address = nes_symbol->address + offset;

        // FIXME: fix this suboptimal implementation
        size_t to_copy = source_symbol->size > nes_symbol->size 
                ? nes_symbol->size 
                : source_symbol->size;

        for (size_t i=0; i < to_copy; i++)
        {
            zenit_nes_program_emit_zpg(target_segment, NES_OP_LDA, (uint8_t)(source_symbol->address + i));
            zenit_nes_program_emit_abs(target_segment, NES_OP_STA, target_address + i);
        }

        if (nes_symbol->size > source_symbol->size)
        {
            zenit_nes_program_emit_imm(target_segment, NES_OP_LDA, 0x0);
            for (size_t i = to_copy; i < nes_symbol->size; i++)
                zenit_nes_program_emit_abs(target_segment, NES_OP_STA, target_address + i);
        }
    }
}

static void emit_data_to_code(struct ZenitNesProgram *program, struct ZenitNesSymbol *source_symbol, struct ZenitNesSymbol *nes_symbol, size_t offset)
{
    if (source_symbol->symkind != nes_symbol->symkind)
        return;

    if (source_symbol->symkind == ZENIT_NES_SYMBOL_ARRAY)
    {
        struct ZenitNesArraySymbol *src_symbol = (struct ZenitNesArraySymbol*) source_symbol;
        struct ZenitNesArraySymbol *dst_symbol = (struct ZenitNesArraySymbol*) nes_symbol;

        for (size_t i=0; i < fl_array_length(dst_symbol->elements); i++)
        {
            struct ZenitNesSymbol *src_elem = src_symbol->elements[i];
            struct ZenitNesSymbol *dst_elem = dst_symbol->elements[i];

            emit_data_to_code(program, src_elem, dst_elem, 0);
        }
    }
    else if (source_symbol->symkind == ZENIT_NES_SYMBOL_STRUCT)
    {
        struct ZenitNesStructSymbol *src_symbol = (struct ZenitNesStructSymbol*) source_symbol;
        struct ZenitNesStructSymbol *dst_symbol = (struct ZenitNesStructSymbol*) nes_symbol;

        for (size_t i=0; i < fl_array_length(dst_symbol->members); i++)
        {
            struct ZenitNesSymbol *src_elem = src_symbol->members[i];
            struct ZenitNesSymbol *dst_elem = dst_symbol->members[i];

            emit_data_to_code(program, src_elem, dst_elem, 0);
        }
    }
    else
    {
        struct ZenitNesCodeSegment *target_segment = program->static_context ? &program->startup : &program->code;
        uint16_t target_address = nes_symbol->address + offset;

        // FIXME: fix this suboptimal implementation
        size_t to_copy = source_symbol->size > nes_symbol->size 
                ? nes_symbol->size 
                : source_symbol->size;

        for (size_t i=0; i < to_copy; i++)
        {
            zenit_nes_program_emit_abs(target_segment, NES_OP_LDA, source_symbol->address + i);
            zenit_nes_program_emit_abs(target_segment, NES_OP_STA, target_address + i);
        }

        if (nes_symbol->size > source_symbol->size)
        {
            zenit_nes_program_emit_imm(target_segment, NES_OP_LDA, 0x0);
            for (size_t i = to_copy; i < nes_symbol->size; i++)
                zenit_nes_program_emit_abs(target_segment, NES_OP_STA, target_address + i);
        }
    }
}

static void emit_code_to_code(struct ZenitNesProgram *program, struct ZenitNesSymbol *source_symbol, struct ZenitNesSymbol *nes_symbol, size_t offset)
{
    if (source_symbol->symkind != nes_symbol->symkind)
        return;

    if (source_symbol->symkind == ZENIT_NES_SYMBOL_ARRAY)
    {
        struct ZenitNesArraySymbol *src_symbol = (struct ZenitNesArraySymbol*) source_symbol;
        struct ZenitNesArraySymbol *dst_symbol = (struct ZenitNesArraySymbol*) nes_symbol;

        for (size_t i=0; i < fl_array_length(dst_symbol->elements); i++)
        {
            struct ZenitNesSymbol *src_elem = src_symbol->elements[i];
            struct ZenitNesSymbol *dst_elem = dst_symbol->elements[i];

            emit_code_to_code(program, src_elem, dst_elem, 0);
        }
    }
    else if (source_symbol->symkind == ZENIT_NES_SYMBOL_STRUCT)
    {
        struct ZenitNesStructSymbol *src_symbol = (struct ZenitNesStructSymbol*) source_symbol;
        struct ZenitNesStructSymbol *dst_symbol = (struct ZenitNesStructSymbol*) nes_symbol;

        for (size_t i=0; i < fl_array_length(dst_symbol->members); i++)
        {
            struct ZenitNesSymbol *src_elem = src_symbol->members[i];
            struct ZenitNesSymbol *dst_elem = dst_symbol->members[i];

            emit_code_to_code(program, src_elem, dst_elem, 0);
        }
    }
    else
    {
        struct ZenitNesCodeSegment *target_segment = program->static_context ? &program->startup : &program->code;
        uint16_t target_address = nes_symbol->address + offset;

        // FIXME: fix this suboptimal implementation
        size_t to_copy = source_symbol->size > nes_symbol->size 
                ? nes_symbol->size 
                : source_symbol->size;

        for (size_t i=0; i < to_copy; i++)
        {
            zenit_nes_program_emit_abs(target_segment, NES_OP_LDA, source_symbol->address + i);
            zenit_nes_program_emit_abs(target_segment, NES_OP_STA, target_address + i);
        }

        if (nes_symbol->size > source_symbol->size)
        {
            zenit_nes_program_emit_imm(target_segment, NES_OP_LDA, 0x0);
            for (size_t i = to_copy; i < nes_symbol->size; i++)
                zenit_nes_program_emit_abs(target_segment, NES_OP_STA, target_address + i);
        }
    }
}

static void emit_code_to_zp(struct ZenitNesProgram *program, struct ZenitNesSymbol *source_symbol, struct ZenitNesSymbol *nes_symbol, size_t offset)
{
    if (source_symbol->symkind != nes_symbol->symkind)
        return;

    if (source_symbol->symkind == ZENIT_NES_SYMBOL_ARRAY)
    {
        struct ZenitNesArraySymbol *src_symbol = (struct ZenitNesArraySymbol*) source_symbol;
        struct ZenitNesArraySymbol *dst_symbol = (struct ZenitNesArraySymbol*) nes_symbol;

        for (size_t i=0; i < fl_array_length(dst_symbol->elements); i++)
        {
            struct ZenitNesSymbol *src_elem = src_symbol->elements[i];
            struct ZenitNesSymbol *dst_elem = dst_symbol->elements[i];

            emit_code_to_zp(program, src_elem, dst_elem, 0);
        }
    }
    else if (source_symbol->symkind == ZENIT_NES_SYMBOL_STRUCT)
    {
        struct ZenitNesStructSymbol *src_symbol = (struct ZenitNesStructSymbol*) source_symbol;
        struct ZenitNesStructSymbol *dst_symbol = (struct ZenitNesStructSymbol*) nes_symbol;

        for (size_t i=0; i < fl_array_length(dst_symbol->members); i++)
        {
            struct ZenitNesSymbol *src_elem = src_symbol->members[i];
            struct ZenitNesSymbol *dst_elem = dst_symbol->members[i];

            emit_code_to_zp(program, src_elem, dst_elem, 0);
        }
    }
    else
    {
        struct ZenitNesCodeSegment *target_segment = program->static_context ? &program->startup : &program->code;
        uint16_t target_address = nes_symbol->address + offset;

        // FIXME: fix this suboptimal implementation
        size_t to_copy = source_symbol->size > nes_symbol->size 
                ? nes_symbol->size 
                : source_symbol->size;

        for (size_t i=0; i < to_copy; i++)
        {
            zenit_nes_program_emit_zpg(target_segment, NES_OP_LDA, (uint8_t)(source_symbol->address + i));
            zenit_nes_program_emit_abs(target_segment, NES_OP_STA, target_address + i);
        }

        if (nes_symbol->size > source_symbol->size)
        {
            zenit_nes_program_emit_imm(target_segment, NES_OP_LDA, 0x0);
            for (size_t i = to_copy; i < nes_symbol->size; i++)
                zenit_nes_program_emit_abs(target_segment, NES_OP_STA, target_address + i);
        }
    }
}

static void emit_code_to_data(struct ZenitNesProgram *program, struct ZenitNesSymbol *source_symbol, struct ZenitNesSymbol *nes_symbol, size_t offset)
{
    if (source_symbol->symkind != nes_symbol->symkind)
        return;

    if (source_symbol->symkind == ZENIT_NES_SYMBOL_ARRAY)
    {
        struct ZenitNesArraySymbol *src_symbol = (struct ZenitNesArraySymbol*) source_symbol;
        struct ZenitNesArraySymbol *dst_symbol = (struct ZenitNesArraySymbol*) nes_symbol;

        for (size_t i=0; i < fl_array_length(dst_symbol->elements); i++)
        {
            struct ZenitNesSymbol *src_elem = src_symbol->elements[i];
            struct ZenitNesSymbol *dst_elem = dst_symbol->elements[i];

            emit_code_to_data(program, src_elem, dst_elem, 0);
        }
    }
    else if (source_symbol->symkind == ZENIT_NES_SYMBOL_STRUCT)
    {
        struct ZenitNesStructSymbol *src_symbol = (struct ZenitNesStructSymbol*) source_symbol;
        struct ZenitNesStructSymbol *dst_symbol = (struct ZenitNesStructSymbol*) nes_symbol;

        for (size_t i=0; i < fl_array_length(dst_symbol->members); i++)
        {
            struct ZenitNesSymbol *src_elem = src_symbol->members[i];
            struct ZenitNesSymbol *dst_elem = dst_symbol->members[i];

            emit_code_to_data(program, src_elem, dst_elem, 0);
        }
    }
    else
    {
        struct ZenitNesCodeSegment *target_segment = program->static_context ? &program->startup : &program->code;
        uint16_t target_address = nes_symbol->address + offset;

        // FIXME: fix this suboptimal implementation
        size_t to_copy = source_symbol->size > nes_symbol->size 
                ? nes_symbol->size 
                : source_symbol->size;

        for (size_t i=0; i < to_copy; i++)
        {
            zenit_nes_program_emit_abs(target_segment, NES_OP_LDA, source_symbol->address + i);
            zenit_nes_program_emit_abs(target_segment, NES_OP_STA, target_address + i);
        }

        if (nes_symbol->size > source_symbol->size)
        {
            zenit_nes_program_emit_imm(target_segment, NES_OP_LDA, 0x0);
            for (size_t i = to_copy; i < nes_symbol->size; i++)
                zenit_nes_program_emit_abs(target_segment, NES_OP_STA, target_address + i);
        }
    }
}

void zenit_nes_emitter_symbol_store(struct ZenitNesProgram *program, struct ZirSymbolOperand *symbol_operand, struct ZenitNesSymbol *nes_symbol, size_t offset)
{
    // The store symbol on a temp symbol is just the assignment of the symbol operand to the "source" property of the temp symbol
    if (nes_symbol->symkind == ZENIT_NES_SYMBOL_TEMP)
    {
        ((struct ZenitNesTempSymbol*) nes_symbol)->source = (struct ZirOperand*) symbol_operand;
        return;
    }

    struct ZenitNesSymbol *source_symbol = fl_hashtable_get(program->symbols, symbol_operand->symbol->name);

    // If the source is a temp symbol, we need to "emit" the store in a special way
    if (source_symbol->symkind == ZENIT_NES_SYMBOL_TEMP)
    {
        zenit_nes_emitter_temp_store(program, (struct ZenitNesTempSymbol*) source_symbol, nes_symbol, offset);
        return;
    }

    uint16_t target_address = nes_symbol->address + offset;

    if (nes_symbol->segment == ZENIT_NES_SEGMENT_ZP)
    {
        if (source_symbol->segment == ZENIT_NES_SEGMENT_ZP)
        {
            emit_zp_to_zp(program, source_symbol, nes_symbol, offset);
        }
        else if (source_symbol->segment == ZENIT_NES_SEGMENT_CODE)
        {
            emit_zp_to_code(program, source_symbol, nes_symbol, offset);
        }
        else if (source_symbol->segment == ZENIT_NES_SEGMENT_DATA)
        {
            emit_zp_to_data(program, source_symbol, nes_symbol, offset);
        }
    }
    else if (nes_symbol->segment == ZENIT_NES_SEGMENT_DATA)
    {
        struct ZenitNesCodeSegment *target_segment = program->static_context ? &program->startup : &program->code;

        if (source_symbol->segment == ZENIT_NES_SEGMENT_DATA)
        {
            emit_data_to_data(program, source_symbol, nes_symbol, offset);
        }
        else if (source_symbol->segment == ZENIT_NES_SEGMENT_ZP)
        {
            emit_data_to_zp(program, source_symbol, nes_symbol, offset);
        }
        else if (source_symbol->segment == ZENIT_NES_SEGMENT_CODE)
        {
            emit_data_to_code(program, source_symbol, nes_symbol, offset);
        }
    }
    else if (nes_symbol->segment == ZENIT_NES_SEGMENT_CODE)
    {
        struct ZenitNesCodeSegment *target_segment = program->static_context ? &program->startup : &program->code;

        if (source_symbol->segment == ZENIT_NES_SEGMENT_CODE)
        {
            emit_code_to_code(program, source_symbol, nes_symbol, offset);
        }
        else if (source_symbol->segment == ZENIT_NES_SEGMENT_ZP)
        {
            emit_code_to_zp(program, source_symbol, nes_symbol, offset);
        }
        else if (source_symbol->segment == ZENIT_NES_SEGMENT_DATA)
        {
            emit_code_to_data(program, source_symbol, nes_symbol, offset);
        }
    }
}
