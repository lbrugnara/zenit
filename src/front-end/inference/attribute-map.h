#ifndef ZENIT_INFER_ATTR_MAP_H
#define ZENIT_INFER_ATTR_MAP_H

#include "infer.h"
#include "../phases/utils.h"

/*
 * Function: visit_attribute_node_map
 *  We iterate over all the attributes to visit all the properties
 *
 * Parameters:
 *  <ZenitContext> *ctx - Context object
 *  <ZenitAttributeNodeMap> *attributes: Attributes map
 * 
 * Returns:
 *  void - This function does not return a value
 *
 * Notes:
 *  By now, this function is pretty dumb because there is nothing to infer in the
 *  properties
 */
static inline void zenit_infer_types_in_attribute_node_map(ZenitContext *ctx, ZenitAttributeNodeMap *attributes)
{
    const char **names = zenit_attribute_node_map_keys(attributes);
    for (size_t i=0; i < fl_array_length(names); i++)
    {
        ZenitAttributeNode *attr = zenit_attribute_node_map_get(attributes, names[i]);

        ZenitPropertyNode **properties = zenit_property_node_map_values(attr->properties);
        for (size_t j=0; j < fl_array_length(properties); j++)
        {
            ZenitPropertyNode *prop = properties[j];

            ZenitSymbol *prop_symbol = zenit_utils_get_tmp_symbol(ctx->program, (ZenitNode*) prop);
            ZenitSymbol *value_symbol = zenit_infer_types_in_node(ctx, prop->value, NULL, ZENIT_INFER_NONE);

            // FIXME: Properties do not have types -so far-
        }

        fl_array_free(properties);
    }

    fl_array_free(names);
}

#endif /* ZENIT_INFER_ATTR_MAP_H */
