#include <fllib.h>

#include "../../Test.h"
#include "../../../src/front-end/phases/infer.h"
#include "../../../src/front-end/phases/parse.h"
#include "../../../src/front-end/phases/resolve.h"
#include "../../../src/front-end/symtable.h"
#include "tests.h"

void zenit_test_infer_errors(void)
{
    const char *source = 
        "var a = 0x1;"                  "\n"
        "var b = cast(a);"              "\n"
        "var c = 0x1FF;"                "\n"
    ;

    struct ZenitContext ctx = zenit_context_new(ZENIT_SOURCE_STRING, source);

    bool valid_parse = zenit_parse_source(&ctx);
    bool valid_resolve = valid_parse ? zenit_resolve_symbols(&ctx) : false;
    bool valid_infer = valid_parse ? zenit_infer_types(&ctx) : false;

    fl_expect("Infer pass must fail with 1 error", !valid_infer && zenit_context_error_count(&ctx) == 1);

    size_t i=0;
    struct FlListNode *tmp = fl_list_head(ctx.errors);
    while (tmp != NULL)
    {
        struct ZenitError *error = (struct ZenitError*) tmp->value;

        fl_vexpect(error->type == ZENIT_ERROR_INFERENCE, 
        "The type of the cast expression cannot be inferred because there is no enough context information (<source>:%u:%u): %s", 
        error->location.line, error->location.col, error->message);

        tmp = tmp->next;
        i++;
    }    

    zenit_context_free(&ctx);
}
