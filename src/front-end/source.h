#ifndef ZENIT_SOURCE_H
#define ZENIT_SOURCE_H

/*
 * Enum: ZenitSourceType 
 *  The source code of a program can be
 *  consumed from a file or a string, so this
 *  enum represents those options,
 *
 */
typedef enum ZenitSourceType {
    ZENIT_SOURCE_FILE,
    ZENIT_SOURCE_STRING
} ZenitSourceType;

/*
 * Struct: ZenitSource
 *  Objects of this type contains the source code of
 *  a program and its length
 */
typedef struct ZenitSource {
    const char *content;
    size_t length;
} ZenitSource;

/*
 * Struct: ZenitSourceLocation
 *  Objects of this type keeps track of the
 *  position in the source code during the compilation
 *  process.
 * 
 * Members:
 *  <const char> *filename: If the source of the program's code is a file, the filename
 *  <unsigned int> line: The line number
 *  <unsigned int> col: The column number
 * 
 */
typedef struct ZenitSourceLocation {
    const char *filename;
    unsigned int line;
    unsigned int col;
} ZenitSourceLocation;

/*
 * Struct: ZenitSourceInfo
 *  Represents the source code of the program and keeps track
 *  of the location during the compilation process
 * 
 * Members:
 *  <ZenitSource> source: An object containing the program's code
 *  <ZenitSourceLocation> location: An object that keeps track of theline and column numbers information
 */
typedef struct ZenitSourceInfo {
    ZenitSource source;
    ZenitSourceLocation location;
} ZenitSourceInfo;

/*
 * Function: zenit_source_new
 *  Creates a <ZenitSourceInfo> object that contains the program's source
 *  and is ready to be used by a <ZenitContext> object.
 *
 * Parameters:
 *  <ZenitSourceType> type: The origin of the source code represented by a <ZenitSourceType> value
 *  <const char> *input: A string that represents a filename or the source code based on the value
 *          of the *type* parameter.
 *
 * Returns:
 *  <ZenitSourceInfo>*: Pointer to an object that is ready to be used by a <ZenitContext>
 *                      object to start the compilation process
 * 
 * Notes:
 *  The pointer returned by this function MUST be freed with the <zenit_source_free> function
 *
 */
ZenitSourceInfo* zenit_source_new(ZenitSourceType type, const char *input);


/*
 * Function: zenit_source_free
 *  Releases the memory used by a <ZenitSourceInfo> object allocated
 *  with the <zenit_source_new> function.
 *
 * Parameters:
 *  <ZenitSourceInfo> *srcinfo: Pointer to a <ZenitSourceInfo> object
 *
 * Returns:
 *  <void>: This function does not return a value
 */
void zenit_source_free(ZenitSourceInfo *srcinfo);

#endif /* ZENIT_SOURCE_H */
