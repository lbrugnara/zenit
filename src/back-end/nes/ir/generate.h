#ifndef ZNES_BUILD_H
#define ZNES_BUILD_H

#include <stdbool.h>
#include "context.h"
#include "../../../zir/program.h"

bool znes_generate_program(ZnesContext *context, ZirProgram *program);

#endif /* ZNES_BUILD_H */
