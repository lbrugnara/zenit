#ifndef ZENIT_GENERATE_H
#define ZENIT_GENERATE_H

#include "../context.h"
#include "../../zir/program.h"

/*
 * Function: zenit_generate_zir
 *  Generate the intermediate representation from the AST and the current
 *  state of the program and type context objects
 *
 * Parameters:
 *  <ZirProgram> *ctx: Context object
 *
 * Returns:
 *  ZirProgram*: The ZIR program
 * 
 * Notes:
 * The object returned by this function must be freed using the
 *  <zir_program_free> function.
 */
ZirProgram* zenit_generate_zir(ZenitContext *ctx);

#endif /* ZENIT_GENERATE_H */
