
#include <fllib/IO.h>
#include <fllib/Cstring.h>
#include "source.h"

static struct ZenitSourceInfo* new_from_file(const char *filename);
static struct ZenitSourceInfo* new_from_string(const char *content);

/*
 * Function: new_from_file
 *  Reads the content of a file and creates a new <struct ZenitSourceInfo>
 *  object that contains it as the program's source code
 *
 * Parameters:
 *  <const char> *filename: A valid filename with source code
 *
 * Returns:
 *  <struct ZenitSourceInfo>*: Represents the program's source code
 *
 */
static struct ZenitSourceInfo* new_from_file(const char *filename)
{
    if (!fl_io_file_exists(filename))
        return NULL;

    const char *content = fl_io_file_read_all_text(filename);

    if (content == NULL)
        return NULL;

    struct ZenitSourceInfo *srcinfo = new_from_string(content);
    fl_cstring_free(content);
    srcinfo->location.filename = fl_cstring_dup(filename);

    return srcinfo;
}

/*
 * Function: new_from_string
 *  Creates a <struct ZenitSourceInfo> object using the *content* parameter
 *  as the program's source code
 *
 * Parameters:
 *  <const char> *content: Program's source code
 *
 * Returns:
 *  <struct ZenitSourceInfo>*: Represents the program's source code
 *
 */
static struct ZenitSourceInfo* new_from_string(const char *content)
{
    if (content == NULL)
        return NULL;

    struct ZenitSourceInfo *srcinfo = fl_malloc(sizeof(struct ZenitSourceInfo));

    srcinfo->source.content = fl_cstring_dup(content);
    srcinfo->source.length = strlen(content);

    srcinfo->location.filename = NULL;
    srcinfo->location.line = 1;
    srcinfo->location.col = 1;

    return srcinfo;
}

/*
 * Function: zenit_source_new
 *  Creates a <struct ZenitSourceInfo> object reading it from a file or by
 *  directly using the *input* parameter. The treatment of the *input*
 *  parameter depends on the origin or *type* of the program's code
 *
 */
struct ZenitSourceInfo* zenit_source_new(enum ZenitSourceType type, const char *input)
{
    if (type == ZENIT_SOURCE_FILE)
        return new_from_file(input);
    
    if (type == ZENIT_SOURCE_STRING)
        return new_from_string(input);

    return NULL;
}

/*
 * Function: zenit_source_free
 *  This function releases all the memory that is allocated
 *  in the <zenit_source_new> function, including the program's
 *  source code and the filename, if present.
 *
 */
void zenit_source_free(struct ZenitSourceInfo *srcinfo)
{
    if (!srcinfo)
        return;

    if (srcinfo->source.content)
        fl_cstring_free(srcinfo->source.content);

    if (srcinfo->location.filename)
        fl_cstring_free(srcinfo->location.filename);

    fl_free(srcinfo);
}
