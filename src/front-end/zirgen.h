#ifndef ZENIT_GENERATE_H
#define ZENIT_GENERATE_H

#include "context.h"
#include "../middle-end/program.h"


struct ZirProgram* zenit_generate_zir(struct ZenitContext *ctx);

#endif /* ZENIT_GENERATE_H */
