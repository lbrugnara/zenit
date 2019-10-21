#ifndef CENIT_SOURCE_H
#define CENIT_SOURCE_H

#include "source.h"

/*
 * Enum: CenitSourceType 
 *  The source code of a program can be
 *  consumed from a file or a string, so this
 *  enum represents those options,
 *
 */
typedef enum CenitSourceType {
    CENIT_SOURCE_FILE,
    CENIT_SOURCE_STRING
} CenitSourceType;

/*
 * Struct: CenitSource
 *  Objects of this type contains the source code of
 *  a program and its length
 */
typedef struct CenitSource {
    const char *content;
    size_t length;
} CenitSource;

/*
 * Struct: CenitSourceLocation
 *  Objects of this type keeps track of the
 *  position in the source code during the compilation
 *  process.
 * 
 * Members:
 *  filename: If the source of the program's code is a file, the filename
 *  line: The line number
 *  col: The column number
 */
typedef struct CenitSourceLocation {
    const char *filename;
    unsigned int line;
    unsigned int col;
} CenitSourceLocation;

/*
 * Struct: CenitSourceInfo
 *  Represents the source code of the program and keeps track
 *  of the location during the compilation process
 * 
 * Members:
 *  source: A <CenitSource> object containing the program's code
 *  location: A <CenitSourceLocation> object that keeps track of the
 *              line and column numbers
 */
typedef struct CenitSourceInfo {
    CenitSource source;
    CenitSourceLocation location;
} CenitSourceInfo;

/*
 * Function: cenit_source_new
 *  Creates a <CenitSourceInfo> object that contains the program's source
 *  and is ready to be used by a <CenitContext> object.
 *
 * Parameters:
 *  type - The origin of the source code represented by a <CenitSourceType> value
 *  input - A string that represents a filename or the source code based on the value
 *          of the *type* parameter.
 *
 * Returns:
 *  CenitSourceInfo* - Pointer to an object that is ready to be used by a <CenitContext>
 *                      object to start the compilation process
 * 
 * Notes:
 *  The pointer returned by this function MUST be freed with the <cenit_source_free> function
 *
 */
CenitSourceInfo* cenit_source_new(CenitSourceType type, const char *input);


/*
 * Function: cenit_source_free
 *  Releases the memory used by a <CenitSourceInfo> object allocated
 *  with the <cenit_source_new> function.
 *
 * Parameters:
 *  srcinfo - Pointer to a <CenitSourceInfo> object
 *
 * Returns:
 *  void - This function does not return a value
 */
void cenit_source_free(CenitSourceInfo *srcinfo);

#endif /* CENIT_SOURCE_H */
