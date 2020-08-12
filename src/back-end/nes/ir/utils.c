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

#include "../../../zir/program.h"

ZnesAllocType znes_utils_variable_type_from_zir_type(ZirType *type)
{
    ZnesAllocType var_type = ZNES_ALLOC_UNK;

    switch (type->typekind)
    {
        case ZIR_TYPE_UINT:
            var_type = ZNES_ALLOC_UINT;
            break;
        case ZIR_TYPE_BOOL:
            var_type = ZNES_ALLOC_BOOL;
            break;
        case ZIR_TYPE_ARRAY:
            var_type = ZNES_ALLOC_ARRAY;
            break;
        case ZIR_TYPE_STRUCT:
            var_type = ZNES_ALLOC_STRUCT;
            break;
        case ZIR_TYPE_REFERENCE:
            var_type = ZNES_ALLOC_REFERENCE;
            break;

        case ZIR_TYPE_NONE: break;
    }

    return var_type;
}

ZnesAllocType znes_utils_variable_type_from_operand(ZnesOperand *operand)
{
    ZnesAllocType var_type = ZNES_ALLOC_UNK;

    switch (operand->type)
    {
        case ZNES_OPERAND_UINT:
            var_type = ZNES_ALLOC_UINT;
            break;

        case ZNES_OPERAND_BOOL:
            var_type = ZNES_ALLOC_BOOL;
            break;

        case ZNES_OPERAND_ARRAY:
            var_type = ZNES_ALLOC_ARRAY;
            break;

        case ZNES_OPERAND_STRUCT:
            var_type = ZNES_ALLOC_STRUCT;
            break;

        case ZNES_OPERAND_VARIABLE:
            var_type = ((ZnesVariableOperand*) operand)->variable->type;
            break;

        case ZNES_OPERAND_REFERENCE:
            var_type = ZNES_ALLOC_REFERENCE;
            break;
    }

    return var_type;
}

ZnesAllocType znes_utils_zir_type_to_nes_type(ZirSymbol *zir_symbol)
{
    if (zir_symbol->name[0] == '%')
        return ZNES_ALLOC_TEMP;

    switch (zir_symbol->type->typekind)
    {
        case ZIR_TYPE_UINT:
            return ZNES_ALLOC_UINT;

        case ZIR_TYPE_BOOL:
            return ZNES_ALLOC_BOOL;

        case ZIR_TYPE_REFERENCE:
            return ZNES_ALLOC_REFERENCE;

        case ZIR_TYPE_ARRAY:
            return ZNES_ALLOC_ARRAY;

        case ZIR_TYPE_STRUCT:
            return ZNES_ALLOC_STRUCT;

        default: break;
    }

    return ZNES_ALLOC_UNK;
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

            size_t member_size = zir_type_size(zir_array_opn->type->member_type, 2 /* bytes */);
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

bool znes_utils_var_alloc_prepare(ZnesProgram *program, ZirBlock *block, ZirSymbol *zir_symbol, ZirAttributeMap *attributes, ZnesAllocRequest *var_alloc)
{
    // Variable kind
    if (zir_symbol->name[0] == '%')
    {
        var_alloc->kind = ZNES_ALLOC_TEMP;
        var_alloc->segment = ZNES_SEGMENT_TEMP;
        var_alloc->is_global = block->type == ZIR_BLOCK_GLOBAL;
        var_alloc->use_address = false;
        var_alloc->size = zir_type_size(zir_symbol->type, 2 /* bytes */);

        // Return, we don't need anything else
        return true;
    }

    switch (zir_symbol->type->typekind)
    {
        case ZIR_TYPE_UINT:
            var_alloc->kind = ZNES_ALLOC_UINT;
            break;

        case ZIR_TYPE_BOOL:
            var_alloc->kind = ZNES_ALLOC_BOOL;
            break;

        case ZIR_TYPE_REFERENCE:
            var_alloc->kind = ZNES_ALLOC_REFERENCE;
            break;

        case ZIR_TYPE_ARRAY:
            var_alloc->kind = ZNES_ALLOC_ARRAY;
            break;

        case ZIR_TYPE_STRUCT:
            var_alloc->kind = ZNES_ALLOC_STRUCT;
            break;

        default: return false;
    }

    // Variable size
    var_alloc->size = zir_type_size(zir_symbol->type, 2 /* bytes */);

    // Segment and address
    var_alloc->is_global = block->type == ZIR_BLOCK_GLOBAL;
    var_alloc->segment = var_alloc->is_global ? ZNES_SEGMENT_DATA : ZNES_SEGMENT_TEXT;

    if (attributes != NULL && zir_attribute_map_has_key(attributes, "NES"))
    {
        ZirAttribute *nes_attribute = zir_attribute_map_get(attributes, "NES");

        if (zir_property_map_has_key(nes_attribute->properties, "segment"))
        {
            ZirProperty *segment_property = zir_property_map_get(nes_attribute->properties, "segment");

            if (segment_property->value->type == ZIR_OPERAND_SYMBOL)
            {
                ZirSymbolOperand *symbol_operand = (ZirSymbolOperand*) segment_property->value;

                if (flm_cstring_equals(symbol_operand->symbol->name, "zp"))
                    var_alloc->segment = ZNES_SEGMENT_ZP;
                else if (flm_cstring_equals(symbol_operand->symbol->name, "data"))
                    var_alloc->segment = ZNES_SEGMENT_DATA;
            }
        }
        else if (zir_property_map_has_key(nes_attribute->properties, "address"))
        {
            ZirProperty *address_property = zir_property_map_get(nes_attribute->properties, "address");
            ZirUintOperand *uint_value = NULL;

            if (address_property->value->type == ZIR_OPERAND_UINT)
            {
                uint_value = (ZirUintOperand*) address_property->value;
            }

            // TODO: ERROR HANDLING
            if (uint_value == NULL)
                return false;

            var_alloc->use_address = true;

            if (uint_value->type->size == ZIR_UINT_8)
            {
                var_alloc->segment = ZNES_SEGMENT_ZP;
                var_alloc->address = uint_value->value.uint8;
            }
            else if (uint_value->type->size == ZIR_UINT_16)
            {
                var_alloc->address = uint_value->value.uint16;

                if (uint_value->value.uint16 >= program->data->base_address)
                {
                    var_alloc->segment = ZNES_SEGMENT_DATA;
                }
                else
                {
                    var_alloc->segment = ZNES_SEGMENT_TEXT;
                }
            }
            
        }
    }

    return true;
}
