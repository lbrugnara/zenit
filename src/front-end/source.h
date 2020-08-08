#ifndef ZENIT_SOURCE_H
#define ZENIT_SOURCE_H

/*
 * Enum: enum ZenitSourceType 
 *  The source code of a program can be
 *  consumed from a file or a string, so this
 *  enum represents those options,
 *
 */
enum ZenitSourceType {
    ZENIT_SOURCE_FILE,
    ZENIT_SOURCE_STRING
};

/*
 * Struct: struct ZenitSource
 *  Objects of this type contains the source code of
 *  a program and its length
 */
struct ZenitSource {
    const char *content;
    size_t length;
};

/*
 * Struct: struct ZenitSourceLocation
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
struct ZenitSourceLocation {
    const char *filename;
    unsigned int line;
    unsigned int col;
};

/*
 * Struct: struct ZenitSourceInfo
 *  Represents the source code of the program and keeps track
 *  of the location during the compilation process
 * 
 * Members:
 *  <struct ZenitSource> source: An object containing the program's code
 *  <struct ZenitSourceLocation> location: An object that keeps track of theline and column numbers information
 */
struct ZenitSourceInfo {
    struct ZenitSource source;
    struct ZenitSourceLocation location;
};

/*
 * Function: zenit_source_new
 *  Creates a <struct ZenitSourceInfo> object that contains the program's source
 *  and is ready to be used by a <ZenitContext> object.
 *
 * Parameters:
 *  <enum ZenitSourceType> type: The origin of the source code represented by a <enum ZenitSourceType> value
 *  <const char> *input: A string that represents a filename or the source code based on the value
 *          of the *type* parameter.
 *
 * Returns:
 *  <struct ZenitSourceInfo>*: Pointer to an object that is ready to be used by a <ZenitContext>
 *                      object to start the compilation process
 * 
 * Notes:
 *  The pointer returned by this function MUST be freed with the <zenit_source_free> function
 *
 */
struct ZenitSourceInfo* zenit_source_new(enum ZenitSourceType type, const char *input);


/*
 * Function: zenit_source_free
 *  Releases the memory used by a <struct ZenitSourceInfo> object allocated
 *  with the <zenit_source_new> function.
 *
 * Parameters:
 *  <struct ZenitSourceInfo> *srcinfo: Pointer to a <struct ZenitSourceInfo> object
 *
 * Returns:
 *  <void>: This function does not return a value
 */
void zenit_source_free(struct ZenitSourceInfo *srcinfo);

#endif /* ZENIT_SOURCE_H */
