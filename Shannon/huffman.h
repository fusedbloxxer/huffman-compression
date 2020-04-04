#ifndef _HUFFMAN_H_
#define _HUFFMAN_H_
#include "hash_table.h"

struct huffman_code
{
    void * key;
    unsigned int key_length;

    void * code;
    unsigned int last_bit;
    unsigned int code_length;
};

struct hash_table * weight_table(const char * filePath, const unsigned int table_size);

struct heap * collect_to_heap(const void * data, const unsigned char collect_method, int (*collector)(struct heap *, const void * data, const unsigned char collect_method));

struct node * huffman_tree(const void * buffer, unsigned int length, const unsigned char collect_method, const unsigned int specifier, const unsigned int table_size, int (*parse_data)(struct hash_table*, const void * const data, const unsigned int length, const unsigned int specifier));

void print_huffman_code(const struct node * const node);

struct hash_table* huffman_hash_table(const void * buffer, const unsigned int length, const unsigned int specifier, struct node ** huffman_root, const unsigned int table_size, int (*parse_data)(struct hash_table*, const void * const data, const unsigned int length, const unsigned int specifier), int (*hash_fun)(const void * const sequence, unsigned int sz));

int huffman_hash(const void * const sequence, unsigned int sz);

int huffman_cmp(const void * const sequence_one, const unsigned int sz_one, const void * const sequence_two, const unsigned int sz_two);

int huffman_encrypt_data(const void * data, const unsigned int data_length, void ** encrypted_data, unsigned int * encrypted_length, struct node ** huffman_root, struct hash_table ** huffman);

int huffman_decrypt_data(const void * data, const unsigned int data_length, void ** decrypted_data, unsigned int * decrypted_length, struct node * huffman_root, struct hash_table * huffman_table, const bool print_flag);

int encode_huffman_file(const char * input_file_name, const char * output_file_name, struct node ** huffman_root, struct hash_table ** huffman_table, const bool print_flag);

int decode_huffman_file(const char * input_file_name, const char * output_file_name, struct node * huffman_root, struct hash_table * huffman_table, const bool print_flag);

#endif // _HUFFMAN_H_
