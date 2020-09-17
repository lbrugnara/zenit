#include <stdint.h>

#include "utils.h"
#include "program.h"
#include "operands/array.h"
#include "operands/bool.h"
#include "operands/reference.h"
#include "operands/struct.h"
#include "operands/uint.h"
#include "operands/variable.h"
#include "objects/alloc.h"
#include "objects/array.h"
#include "objects/bool.h"
#include "objects/reference.h"
#include "objects/struct.h"
#include "objects/temp.h"
#include "objects/uint.h"

#include "../nes.h"
#include "../../../zir/program.h"

ZnesAllocType znes_alloc_type_from_zir_type(ZirType *type)
{
    ZnesAllocType var_type = ZNES_ALLOC_TYPE_UNK;

    switch (type->typekind)
    {
        case ZIR_TYPE_UINT:
            var_type = ZNES_ALLOC_TYPE_UINT;
            break;
        case ZIR_TYPE_BOOL:
            var_type = ZNES_ALLOC_TYPE_BOOL;
            break;
        case ZIR_TYPE_ARRAY:
            var_type = ZNES_ALLOC_TYPE_ARRAY;
            break;
        case ZIR_TYPE_STRUCT:
            var_type = ZNES_ALLOC_TYPE_STRUCT;
            break;
        case ZIR_TYPE_REFERENCE:
            var_type = ZNES_ALLOC_TYPE_REFERENCE;
            break;

        case ZIR_TYPE_NONE: break;
    }

    return var_type;
}

ZnesAllocType znes_utils_variable_type_from_operand(ZnesOperand *operand)
{
    ZnesAllocType var_type = ZNES_ALLOC_TYPE_UNK;

    switch (operand->type)
    {
        case ZNES_OPERAND_UINT:
            var_type = ZNES_ALLOC_TYPE_UINT;
            break;

        case ZNES_OPERAND_BOOL:
            var_type = ZNES_ALLOC_TYPE_BOOL;
            break;

        case ZNES_OPERAND_ARRAY:
            var_type = ZNES_ALLOC_TYPE_ARRAY;
            break;

        case ZNES_OPERAND_STRUCT:
            var_type = ZNES_ALLOC_TYPE_STRUCT;
            break;

        case ZNES_OPERAND_VARIABLE:
            var_type = ((ZnesVariableOperand*) operand)->variable->type;
            break;

        case ZNES_OPERAND_REFERENCE:
            var_type = ZNES_ALLOC_TYPE_REFERENCE;
            break;
    }

    return var_type;
}

ZnesAllocType znes_utils_zir_type_to_nes_type(ZirSymbol *zir_symbol)
{
    if (zir_symbol->name[0] == '%')
        return ZNES_ALLOC_TYPE_TEMP;

    switch (zir_symbol->type->typekind)
    {
        case ZIR_TYPE_UINT:
            return ZNES_ALLOC_TYPE_UINT;

        case ZIR_TYPE_BOOL:
            return ZNES_ALLOC_TYPE_BOOL;

        case ZIR_TYPE_REFERENCE:
            return ZNES_ALLOC_TYPE_REFERENCE;

        case ZIR_TYPE_ARRAY:
            return ZNES_ALLOC_TYPE_ARRAY;

        case ZIR_TYPE_STRUCT:
            return ZNES_ALLOC_TYPE_STRUCT;

        default: break;
    }

    return ZNES_ALLOC_TYPE_UNK;
}

static inline ZnesOperand* internal_make_nes_operand(ZnesProgram *program, ZirOperand *zir_opn)
{
    switch (zir_opn->type)
    {
        case ZIR_OPERAND_UINT:
            ZirUintOperand *zir_uint_opn = (ZirUintOperand*) zir_opn;
            
            ZnesUintValue value = { 0 };
            ZnesUintSize size = ZNES_UINT_UNK;
            switch (zir_uint_opn->type->size)
            {
                case ZIR_UINT_8:
                    size = ZNES_UINT_8;
                    value.uint8 = zir_uint_opn->value.uint8;
                    break;

                case ZIR_UINT_16:
                    size = ZNES_UINT_16;
                    value.uint16 = zir_uint_opn->value.uint16;
                    break;

                default: break;
            }
            
            return (ZnesOperand*) znes_uint_operand_new(size, value);

        case ZIR_OPERAND_BOOL:
            ZirBoolOperand *zir_bool_opn = (ZirBoolOperand*) zir_opn;

            return (ZnesOperand*) znes_bool_operand_new(zir_bool_opn->value);

        case ZIR_OPERAND_ARRAY:
            ZirArrayOperand *zir_array_opn = (ZirArrayOperand*) zir_opn;

            size_t member_size = zir_type_size(zir_array_opn->type->member_type, ZNES_POINTER_SIZE);
            size_t length = zir_array_opn->type->length;

            ZnesArrayOperand *array_operand = znes_array_operand_new(member_size, length);

            for (size_t i=0; i < fl_array_length(zir_array_opn->elements); i++)
                array_operand->elements[i] = znes_utils_make_nes_operand(program, zir_array_opn->elements[i]);

            return (ZnesOperand*) array_operand;

        case ZIR_OPERAND_STRUCT:
            ZirStructOperand *zir_struct_opn = (ZirStructOperand*) zir_opn;

            ZnesStructOperand *struct_operand = znes_struct_operand_new();

            for (size_t i = 0; i < fl_array_length(zir_struct_opn->members); i++)
            {
                ZirStructOperandMember *member = zir_struct_opn->members[i];
                znes_struct_operand_add_member(struct_operand, member->name, znes_utils_make_nes_operand(program, member->operand));
            }

            return (ZnesOperand*) struct_operand;

        case ZIR_OPERAND_SYMBOL:
            ZirSymbolOperand *zir_symbol_opn = (ZirSymbolOperand*) zir_opn;

            ZnesAlloc *variable = fl_hashtable_get(program->variables, zir_symbol_opn->symbol->name);

            return (ZnesOperand*) znes_variable_operand_new(variable);

        case ZIR_OPERAND_REFERENCE:
            ZirReferenceOperand *zir_ref_opn = (ZirReferenceOperand*) zir_opn;

            ZnesOperand *var_operand = znes_utils_make_nes_operand(program, (ZirOperand*) zir_ref_opn->operand);
            
            if (var_operand->type == ZNES_OPERAND_VARIABLE)
                return (ZnesOperand*) znes_reference_operand_new((ZnesVariableOperand*) var_operand);

            return NULL;
    }

    return NULL;
}

ZnesOperand* znes_utils_make_nes_operand(ZnesProgram *program, ZirOperand *zir_opn)
{
    ZnesOperand *operand = internal_make_nes_operand(program, zir_opn);

    znes_operand_pool_register(program->operands, operand);

    return operand;
}

bool znes_alloc_request_init(ZnesProgram *znes_program, ZirBlock *zir_block, ZirSymbol *zir_symbol, ZirAttributeMap *zir_attributes, ZnesAllocRequest *znes_alloc_request)
{
    if (zir_symbol->name[0] == '%')
    {
        // Symbols starting with '%' are temporal symbols
        znes_alloc_request->type = ZNES_ALLOC_TYPE_TEMP;
        znes_alloc_request->segment = ZNES_SEGMENT_TEMP;
        znes_alloc_request->is_global = zir_block->type == ZIR_BLOCK_GLOBAL;
        znes_alloc_request->use_address = false;
        znes_alloc_request->size = zir_type_size(zir_symbol->type, ZNES_POINTER_SIZE);

        // Return, we don't need anything else
        return true;
    }

    // This defines the allocation structure
    switch (zir_symbol->type->typekind)
    {
        case ZIR_TYPE_UINT:
            znes_alloc_request->type = ZNES_ALLOC_TYPE_UINT;
            break;

        case ZIR_TYPE_BOOL:
            znes_alloc_request->type = ZNES_ALLOC_TYPE_BOOL;
            break;

        case ZIR_TYPE_REFERENCE:
            znes_alloc_request->type = ZNES_ALLOC_TYPE_REFERENCE;
            break;

        case ZIR_TYPE_ARRAY:
            znes_alloc_request->type = ZNES_ALLOC_TYPE_ARRAY;
            break;

        case ZIR_TYPE_STRUCT:
            znes_alloc_request->type = ZNES_ALLOC_TYPE_STRUCT;
            break;

        default: return false;
    }

    // Allocation size based on the type
    znes_alloc_request->size = zir_type_size(zir_symbol->type, ZNES_POINTER_SIZE);

    // Segment and address
    znes_alloc_request->is_global = zir_block->type == ZIR_BLOCK_GLOBAL;
    znes_alloc_request->segment = znes_alloc_request->is_global ? ZNES_SEGMENT_DATA : ZNES_SEGMENT_TEXT;

    // The segment and the address can be changed using attributes
    if (zir_attributes != NULL && zir_attribute_map_has_key(zir_attributes, "NES"))
    {
        ZirAttribute *nes_attribute = zir_attribute_map_get(zir_attributes, "NES");

        // If the "segment" property is present in the NES attribute, it will define the
        // target segment of the allocation
        if (zir_property_map_has_key(nes_attribute->properties, "segment"))
        {
            ZirProperty *segment_property = zir_property_map_get(nes_attribute->properties, "segment");

            if (segment_property->value->type != ZIR_OPERAND_SYMBOL)
            {
                // TODO: Error handling, we don't support other operands for segment
                return false;
            }

            ZirSymbolOperand *symbol_operand = (ZirSymbolOperand*) segment_property->value;

            if (flm_cstring_equals(symbol_operand->symbol->name, "zp"))
            {
                znes_alloc_request->segment = ZNES_SEGMENT_ZP;
            }
            else if (flm_cstring_equals(symbol_operand->symbol->name, "data"))
            {
                znes_alloc_request->segment = ZNES_SEGMENT_DATA;
            }
            else
            {
                // TODO: Error handling, we don't support other segments yet
                return false;
            }
        }
        else if (zir_property_map_has_key(nes_attribute->properties, "address"))
        {
            // The address property is a uint value
            ZirUintOperand *uint_value = NULL;
            ZirProperty *address_property = zir_property_map_get(nes_attribute->properties, "address");

            if (address_property->value->type == ZIR_OPERAND_UINT)
            {
                uint_value = (ZirUintOperand*) address_property->value;
            }
            else
            {
                // TODO: Error handling, we don't support other types
                return false;
            }

            znes_alloc_request->use_address = true;

            if (uint_value->type->size == ZIR_UINT_8)
            {
                // If the address is a uint8 the segment is ZEROPAGE
                znes_alloc_request->address = uint_value->value.uint8;
                znes_alloc_request->segment = ZNES_SEGMENT_ZP;
            }
            else if (uint_value->type->size == ZIR_UINT_16)
            {
                znes_alloc_request->address = uint_value->value.uint16;

                if (uint_value->value.uint16 >= znes_program->data->base_address)
                {
                    // If the address is within the PRG-ROM it is DATA segment
                    znes_alloc_request->segment = ZNES_SEGMENT_DATA;
                }
                else
                {
                    // Values lesser than the PRG-ROM base address are placed
                    // in the TEXT segment (RAM)
                    znes_alloc_request->segment = ZNES_SEGMENT_TEXT;
                }
            }
            else
            {
                // TODO: Error handling, we don't support other types yet
                return false;
            }
            
        }
    }

    return true;
}
