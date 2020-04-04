#ifndef _HASH_TABLE_H_
#define _HASH_TABLE_H_
#include "avl_tree.h"

struct hash_table
{
    /// NUMBER OF ELEMENTS IN THE TABLE
    unsigned int element_count;

    /// THE SIZE OF THE HASH TABLE
    unsigned int table_size;

    /// THE TREES CONTAINED
    struct avl_tree* trees;
};

/**
*   @PARAMS
*   sequence - Memory address of data
*         sz - In bytes
*
*   @RETURN
*   NULL_ARGUMENT - sequence is NULL
*   >= 0 - Hash-code
*/
int hash_code(const void * const sequence, unsigned int sz);

/**
*   @PARAMS
*   sequence_one - Memory address of the first sequence
*         sz_one - Size of the first sequence in bytes
*   sequence_two - Memory address of the second sequence
*         sz_two - Size of the second sequence in bytes
*
*   @RETURN
*   NULL_ARGUMENT - sequence_one or sequence_two is NULL
*   -1  - First sequence is "smaller" than the second
*    0  - The sequences are equal
*    1  - First sequence is "bigger" than the second
*/
int seq_cmp(const void * const sequence_one, const unsigned int sz_one, const void * const sequence_two, const unsigned int sz_two);

/**
*   @PARAMS
*   table    - The hash-table in which the element/tree will be searched
*   element  - Element to be found
*   length   - In bytes
*   hash_fun - Hashing function
*   compare  - Comparison function
*
*   @RETURN
*    NULL - No node was found with the same value as element
*   !NULL - Node containing the element is found
*/
struct node * const find_by_kv(const struct hash_table* const table, const void * const element, const unsigned int length, int (*hash_fun)(const void * const seq, unsigned int sz), int (*compare)(const void * const, const unsigned int, const void * const, const unsigned int));

/**
*   @PARAMS
*   table    - The hash-table in which the element/tree will be searched
*   element  - Element to be found
*   length   - In bytes
*   hash_fun - Hashing function
*
*   @RETURN
*    NULL - No node was found with the same value as element
*   !NULL - Node containing the element is found
*/
struct avl_tree * const find_by_key(const struct hash_table* const table, const void * const element, const unsigned int length, int (*hash_fun)(const void * const seq, unsigned int sz));

/**
*   @PARAMS
*   table      - Memory address of data
*   table_size - In bytes
*
*   @RETURN
*   BAD_MEMORY_ALLOC - Could not allocate memory for the trees
*   NULL_ARGUMENT    - Argument table is NULL
*   STATUS_SUCCESS   - Hash table successfully created
*/
int create_table(struct hash_table* table, const unsigned int table_size);

/**
*   @PARAMS
*   table    - Memory address of data
*   element  - New element to be added
*   length   - In bytes
*   hash_fun - Hash function
*
*   @RETURN
*   NULL_ARGUMENT    - Comparison function is NULL
*   NULL_ARGUMENT    - Trees array is NULL
*   NULL_ARGUMENT    - Hash function is NULL
*   NULL_ARGUMENT    - Argument table is NULL
*   BAD_MEMORY_ALLOC - Could not allocate memory for the new element
*   STATUS_SUCCESS   - Element successfully added
*/
int add_element(struct hash_table* table, const void * const element, const unsigned int length, int (*hash_fun)(const void * const seq, unsigned int sz), int (*compare)(const void * const, const unsigned int, const void * const, const unsigned int));

/**
*   @PARAMS
*   table   - Memory address of data
*
*   @RETURN
*   NULL_ARGUMENT  - Argument table is NULL
*   STATUS_SUCCESS - Hash table successfully cleaned
*/
int clean_table(struct hash_table* table);

void print_table(struct hash_table* table, void (*printer)(const struct node * const));

struct node * const find_first_by_value(const struct hash_table * const table, const void * const element, const unsigned int length, int (*compare)(const void * const, const unsigned int, const void * const, const unsigned int));
#endif // _HASH_TABLE_H_
