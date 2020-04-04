#ifndef _SHANNON_H_
#define _SHANNON_H_
#include "hash_table.h"

/**
*   @PARAMS
*   filePath  - Path to the file containing the data
*   specifier - The length of the sequences that are to be counted
*   event - Pointer to a function that calculates the entropy of the data
*
*   @RETURN
*   FILE_ERROR - sequence_one or sequence_two is NULL
*   entropy    - Shannon Information
*/
double shannon_entropy(const char * filePath, const unsigned int specifier, double (*event)(const void * const data, const unsigned int length, const unsigned int specifier));

/**
*   @PARAMS
*   data       - Memory address of data
*   length     - In bytes
*   specifier  - The length of the sequences that are to be counted
*   table_size - Size of the hash-table
*
*   @RETURN
*   entropy    - Shannon Information
*/
double sequence_entropy(const void * const data, const unsigned int length, const unsigned int specifier, const unsigned int table_size);

/**
*   @PARAMS
*   data       - Memory address of data
*   length     - In bytes
*   specifier  - The length of the sequences that are to be counted
*   table_size - Size of the hash-table
*
*   @RETURN
*   entropy    - Shannon Information
*/
double words_entropy(const void * const data, const unsigned int length, const unsigned int specifier, const unsigned int table_size);

struct hash_table * weight_seq_table(const void * const data, const unsigned int length, const unsigned int specifier, const unsigned int table_size);

int parse_words(struct hash_table * table, const void * const data, const unsigned int length, const unsigned int specifier);

int parse_sequences(struct hash_table * table, const void * const data, const unsigned int length, const unsigned int specifier);

int parse_words(struct hash_table * table, const void * const data, const unsigned int length, const unsigned int specifier);

struct hash_table * frequency_hash_table(const void * const data, const unsigned int length, const unsigned int specifier, const unsigned int table_size, int (*parse_data)(struct hash_table*, const void * const data, const unsigned int length, const unsigned int specifier));

#endif // _SHANNON_H_
