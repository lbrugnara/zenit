#ifndef ZNES_BUILD_H
#define ZNES_BUILD_H

#include <stdbool.h>
#include "../../../zir/program.h"
#include "program.h"

ZnesProgram* znes_generate_program(ZirProgram *program, bool scripting);

#endif /* ZNES_BUILD_H */
