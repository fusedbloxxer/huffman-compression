#ifndef _UTILITIES_H_
#define _UTILITIES_H_
#include <stdbool.h>

enum error_codes
{
    NULL_RESULT         = -105,
    INVALID_TYPE        = -104,
    FILE_ERROR          = -103,
    BAD_MEMORY_ALLOC    = -102,
    NULL_ARGUMENT       = -101,
    INVALID_FORMAT      = -100,
    STATUS_SUCCESS      = 0
};

enum data_format
{
    FORMAT_ASCII = 0,
    FORMAT_BITS  = 1,
    FORMAT_BYTES = 2
};

/**
*   @PARAMS
*   filePath - Path to the file containing the data
*   data     - Double pointer to create buffer and export it ouside
*   length   - Pointer to a length variable (bytes)
*
*   @RETURN
*   FILE_ERROR - sequence_one or sequence_two is NULL
*   BAD_MEMORY_ALLOC - Could not allocate data
*   entropy    - Shannon Information
*/
int fetch_data(const char * const filePath, void ** buffer, unsigned int * length, const char * read_format);

int write_data(const char * const filePath, const void * buffer, const unsigned int length);

/**
*   @PARAMS
*   data   - Memory address of data
*   sz     - In bytes
*   format - FORMAT_ASCII, FORMAT_BITS, FORMAT_BYTE
*
*   @RETURN
*   NULL_ARGUMENT  - Data is NULL
*   INVALID_FORMAT - Invalid format was specified
*   STATUS_SUCCESS - Successfully printed the data
*/
int print_bits(const void * const data, const unsigned int sz, const unsigned int format, const bool little_endian);

#endif // _UTILITIES_H_
