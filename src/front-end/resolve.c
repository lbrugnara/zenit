#include "resolve.h"
#include "symbol.h"

/*
 * Type: CenitResolver
 *  A resolver function takes the <CenitSymbolTable> and a <CenitNode> and
 *  populates the table (if needed) with symbol information.
 *  The function returns a <CenitSymbol> if the node updates the table or NULL
 *  if there is nothing to update
 */
typedef CenitSymbol*(*CenitResolver)(CenitSymbolTable *symtable, CenitNode *node);

/*
 * Function: visit_node
 *  This function selects the visitor function based on the node's type
 *  and calls the function.
 *
 * Parameters:
 *  symtable - Symbol table
 *  node - Node to visit
 *
 * Returns:
 *  CenitSymbol* - A valid symbol if the visited node (or nodes) defines one
 *                  or NULL
 *
 */
static CenitSymbol* visit_node(CenitSymbolTable *symtable, CenitNode *node);


/*
 * Function: visit_nothing
 *  Some leafs in the <CenitAst> do not perform symbol-related actions to this
 *  function simply returns NULL
 *
 * Parameters:
 *  symtable - Symbol table
 *  node - Node to visit
 *
 * Returns:
 *  CenitSymbol* - A valid symbol if the visited node (or nodes) defines one
 *                  or NULL
 *
 */
static CenitSymbol* visit_nothing(CenitSymbolTable *symtable, CenitNode *node);

/*
 * Function: visit_variable
 *  Adds a new symbol in the table based on the <CenitVariableNode> information
 *  present in the <CenitAst>
 *
 * Parameters:
 *  symtable - Symbol table
 *  node - Node to visit
 *
 * Returns:
 *  CenitSymbol* - A valid symbol if the visited node (or nodes) defines one
 *                  or NULL
 *
 */
static CenitSymbol* visit_variable(CenitSymbolTable *symtable, CenitNode *node);

/*
 * Variable: resolvers
 *  An array indexed with a <CenitNodeType> to get a <CenitResolver> function
 */
static const CenitResolver resolvers[] = {
    [CENIT_NODE_VARIABLE] = &visit_variable,

    // These nodes do not play a role in this phase
    [CENIT_NODE_LITERAL] = &visit_nothing,
    [CENIT_NODE_ARRAY_INIT] = &visit_nothing,
    
};

static CenitSymbol* visit_node(CenitSymbolTable *symtable, CenitNode *node)
{
    return resolvers[node->type](symtable, node);
}

static CenitSymbol* visit_nothing(CenitSymbolTable *symtable, CenitNode *node)
{
    return NULL;
}

static CenitSymbol* visit_variable(CenitSymbolTable *symtable, CenitNode *node)
{
    CenitVariableNode *var_decl = (CenitVariableNode*)node;

    // We don't care about the returned symbol -if any- at this phase
    visit_node(symtable, var_decl->value);

    // Create and insert the symbol in the table
    return cenit_symtable_add(symtable, cenit_symbol_new(var_decl->name, &var_decl->typeinfo));
}

/*
 * Function: cenit_resolve_symbols
 *  We just iterate over the declarations visiting each node
 */
void cenit_resolve_symbols(CenitSymbolTable *symtable, CenitAst *ast)
{
    if (!ast || !ast->decls)
        return;

    for (size_t i=0; i < fl_array_length(ast->decls); i++)
    {
        CenitNode *node = ast->decls[i];
        
        visit_node(symtable, node);
    }
}
