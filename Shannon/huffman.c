#include "hash_table.h"
#include "utilities.h"
#include "huffman.h"
#include "shannon.h"
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "heap.h"

struct hash_table * weight_table(const char * filePath, const unsigned int table_size)
{
    void * buffer;
    unsigned int length;

    if (fetch_data(filePath, &buffer, &length, "r") != STATUS_SUCCESS)
    {
        return NULL;
    }

    struct hash_table * table = frequency_hash_table(buffer, length, 1, table_size, &parse_sequences);
    free(buffer);

    return table;
}

struct heap * collect_to_heap(const void * data, const unsigned char collect_method, int (*collector)(struct heap *, const void * data, const unsigned char collect_method))
{
    if (data == NULL || collector == NULL)
    {
        return NULL;
    }

    struct heap *heap = (struct heap *)malloc(sizeof(struct heap));

    if (heap == NULL)
    {
        return NULL;
    }

    if (collector(heap, data, collect_method) != STATUS_SUCCESS)
    {
        free(heap);
        return NULL;
    }

    return heap;
}

int transfer(struct heap *heap, struct node ** node)
{
    if (node != NULL && *node != NULL)
    {
        if ((*node)->left_child != NULL)
        {
            transfer(heap, &(*node)->left_child);
        }

        if ((*node)->right_child != NULL)
        {
            transfer(heap, &(*node)->right_child);
        }

        if (heap->weak_pointer == DEEP_COLLECTION)
        {
            struct node new_node;

            new_node.info.length = (*node)->info.length;
            new_node.info.sequence = (void *)malloc(new_node.info.length);

            if (new_node.info.sequence == NULL)
            {
                return BAD_MEMORY_ALLOC;
            }

            new_node.info.sequence = (void *)memcpy(new_node.info.sequence, (*node)->info.sequence, new_node.info.length);
            new_node.info.total = (*node)->info.total;
            new_node.right_child = NULL;
            new_node.left_child = NULL;
            new_node.bal = 0;

            int result = heap->push(heap, &new_node);

            if (result != STATUS_SUCCESS)
            {
                free(new_node.info.sequence);
                return result;
            }
        }
        else
        {
            int result = heap->push(heap, *node);
            (*node)->right_child = NULL;
            (*node)->left_child = NULL;
            (*node)->bal = 0;

            if (result != STATUS_SUCCESS)
            {
                return result;
            }
            *node = NULL;
        }
    }
    return STATUS_SUCCESS;
}

int hash_table_to_heap(struct heap* heap, const void *data, const unsigned char collect_method)
{
    if (heap == NULL || data == NULL)
    {
        return NULL_ARGUMENT;
    }

    if (collect_method != WEAK_COLLECTION && collect_method != DEEP_COLLECTION)
    {
        return INVALID_TYPE;
    }

    struct hash_table* hash_table = (struct hash_table *)data;

    {
        int result = create_heap(heap, sizeof(struct node), collect_method, print, compare_weights);
        if (result != STATUS_SUCCESS)
        {
            return result;
        }
    }

    if (hash_table->trees != NULL)
    {
        for (unsigned int i = 0; i < hash_table->table_size; ++i)
        {
            transfer(heap, &(hash_table->trees + i)->root);
        }

        if (collect_method == WEAK_COLLECTION)
        {
            int result = clean_table(hash_table);
            if (result != STATUS_SUCCESS)
            {
                return result;
            }
        }
    }

    return STATUS_SUCCESS;
}

struct node * huffman_tree(const void * buffer, unsigned int length, const unsigned char collect_method, const unsigned int specifier, const unsigned int table_size, int (*parse_data)(struct hash_table*, const void * const data, const unsigned int length, const unsigned int specifier))
{
    if (buffer == NULL || length <= 0)
    {
        return NULL;
    }

    struct heap * heap = NULL;

    {
        /// Obtain hash_table with weights then transfer it to a heap.
        struct hash_table* hash_table = frequency_hash_table(buffer, length, specifier, table_size, parse_data);
        heap = collect_to_heap(hash_table, collect_method, &hash_table_to_heap);
        clean_table(hash_table);
        free(hash_table);
    }

    {
        /// SCOPE LIMITER FOR pairs
        struct node * pair;
        struct node static_pair;

        if (collect_method == DEEP_COLLECTION)
        {
            pair = &static_pair;
        }

        while (heap->element_count != 1)
        {
            if (collect_method == WEAK_COLLECTION && (pair = (struct node *)malloc(sizeof(struct node))) == NULL)
            {
                return NULL;
            }

            pair->info.length = 0;
            pair->info.sequence = NULL;

            struct node * first = NULL;
            struct node * secon = NULL;

            heap->pop(heap, (void **)&first);
            heap->pop(heap, (void **)&secon);

            pair->left_child = first;
            pair->right_child = secon;

            pair->info.total = first->info.total + secon->info.total;
            pair->bal = first->bal + secon->bal + 2;

            heap->push(heap, pair);
        }
    }

    struct node * huffman_root = *heap->elements;

    *heap->elements = NULL;

    /// Add the root too !
    ++huffman_root->bal;

    clean_heap(heap);
    free(heap);

    return huffman_root;
}

int print_breadth_first(const struct node * node)
{
    if (node != NULL)
    {
        unsigned int queue_first = 0;
        unsigned int queue_last  = 1;
        unsigned int queue_total = node->bal;

        const struct node  ** queue = (const struct node **)malloc(sizeof(struct node *) * queue_total);
        unsigned int  * depth_level = (unsigned int *)calloc(queue_total, sizeof(unsigned int));
        int count = 0, previous_level = -1;
        *queue = node;

        while (queue_first < queue_total)
        {
            node = *(queue + queue_first);

            if (previous_level < 0 || depth_level[queue_first] != depth_level[previous_level])
            {
                printf("\n === LEVEL %d:\n", depth_level[queue_first]);
                previous_level = queue_first;
            }

            print_node(node);

            ++count;

            if (node->left_child != NULL)
            {
                depth_level[queue_last] = depth_level[previous_level] + 1;
                *(queue + queue_last++) = node->left_child;
            }

            if (node->right_child != NULL)
            {
                depth_level[queue_last] = depth_level[previous_level] + 1;
                *(queue + queue_last++) = node->right_child;
            }

            ++queue_first;
        }

        printf("TOTAL: %d", count);

        free(depth_level);
        free(queue);
    }
    return STATUS_SUCCESS;
}

int create_huffman_code(struct huffman_code * huffman_pair, void * key_data, unsigned int key_data_length, void * code_data, const unsigned int code_data_length, const unsigned int bit_offset)
{
    if (key_data == NULL || code_data == NULL)
    {
        return NULL_ARGUMENT;
    }

    if ((huffman_pair->key = (void *)malloc(key_data_length)) == NULL)
    {
        return BAD_MEMORY_ALLOC;
    }

    if ((huffman_pair->code = (void *)malloc(code_data_length)) == NULL)
    {
        free(huffman_pair->key);
        return BAD_MEMORY_ALLOC;
    }

    huffman_pair->last_bit = bit_offset;
    huffman_pair->key_length  = key_data_length;
    huffman_pair->code_length = code_data_length;
    huffman_pair->key = (void *)memcpy(huffman_pair->key, key_data, key_data_length);
    huffman_pair->code = (void *)memcpy(huffman_pair->code, code_data, code_data_length);
    return STATUS_SUCCESS;
}

int huffman_cmp(const void * const sequence_one, const unsigned int sz_one, const void * const sequence_two, const unsigned int sz_two)
{
    if (sequence_one == NULL || sequence_two == NULL)
    {
        return NULL_ARGUMENT;
    }

    if (sz_one == sizeof(struct huffman_code) && sz_two == sizeof(struct huffman_code))
    {
        struct huffman_code * first  = (struct huffman_code *)sequence_one;
        struct huffman_code * second = (struct huffman_code *)sequence_two;
        return seq_cmp(first->code, first->code_length, second->code, second->code_length);
    }
    else
    {
        struct huffman_code * second = (struct huffman_code *)sequence_two;
        return seq_cmp((const char *)sequence_one, sz_one, second->code, second->code_length);
    }
}

int huffman_hash(const void * const sequence, unsigned int sz)
{
    if (sequence == NULL || sz == 0)
    {
        return NULL_ARGUMENT;
    }

    if (sz == sizeof(struct huffman_code))
    {
        struct huffman_code* huffman_code = (struct huffman_code *)sequence;
        return hash_code(huffman_code->key, huffman_code->key_length);
    }
    else
    {
        return hash_code((const char *)sequence, sz);
    }
}

int build_huffman_hash_table(const struct node * node, struct hash_table* table, unsigned char * codes, const unsigned int byte_offset, const unsigned char bit_offset, int (*hash_fun)(const void * const sequence, unsigned int sz))
{
    if (node != NULL)
    {
        if (node->left_child == NULL && node->right_child == NULL)
        {
            struct huffman_code huffman_code;
            create_huffman_code(&huffman_code, node->info.sequence, node->info.length, codes, byte_offset + 1, bit_offset);
            add_element(table, &huffman_code, sizeof(struct huffman_code), hash_fun, &huffman_cmp);
            return STATUS_SUCCESS;
        }

        unsigned int next_byte_offset;
        unsigned char next_bit_offset;

        if (bit_offset == 128)
        {
            next_byte_offset = byte_offset + 1;
            next_bit_offset = 1;
        }
        else
        {
            next_byte_offset = byte_offset;
            next_bit_offset = bit_offset << 1;
        }

        if (node->left_child != NULL)
        {
            *(codes + byte_offset) &= ~bit_offset;
            build_huffman_hash_table(node->left_child, table, codes, next_byte_offset, next_bit_offset, hash_fun);
        }

        if (node->right_child != NULL)
        {
            *(codes + byte_offset) |= bit_offset;
            build_huffman_hash_table(node->right_child, table, codes, next_byte_offset, next_bit_offset, hash_fun);
            *(codes + byte_offset) &= ~bit_offset;
        }

    }
    return STATUS_SUCCESS;
}

struct hash_table* huffman_hash_table(const void * buffer, const unsigned int length, const unsigned int specifier, struct node ** huffman_root, const unsigned int table_size, int (*parse_data)(struct hash_table*, const void * const data, const unsigned int length, const unsigned int specifier), int (*hash_fun)(const void * const sequence, unsigned int sz))
{
    *huffman_root = huffman_tree(buffer, length, WEAK_COLLECTION, specifier, table_size, parse_data);

    if (*huffman_root == NULL || (*huffman_root)->bal == 0)
    {
        clean_nodes(huffman_root);
        free(*huffman_root);
        return NULL;
    }

    struct hash_table * huffman_table = (struct hash_table *)malloc(sizeof(struct hash_table));
    unsigned char  * codes;

    if (huffman_table == NULL
            || create_table(huffman_table, table_size) != STATUS_SUCCESS
            || (codes = (unsigned char *)calloc((*huffman_root)->bal / 8 + 1, sizeof(unsigned char))) == NULL)
    {
        clean_nodes(huffman_root);
        free(*huffman_root);
        free(huffman_table);
        return NULL;
    }

    build_huffman_hash_table(*huffman_root, huffman_table, codes, 0, 1, hash_fun);
    free(codes);

    return huffman_table;
}

void print_huffman_code(const struct node * const node)
{
    if (node != NULL)
    {
        struct huffman_code * huffman_pair = (struct huffman_code *)node->info.sequence;

        print_bits(huffman_pair->key, huffman_pair->key_length, FORMAT_ASCII, true);
        printf(": ");

        for (unsigned int byte = 0; byte < huffman_pair->code_length - 1; ++byte)
        {
            for (unsigned char bit = 1; bit != 0; bit <<= 1)
            {
                printf("%d", (*((unsigned char *)huffman_pair->code + byte) & bit) != 0);
            }
        }

        for (unsigned char bit = 1; bit < huffman_pair->last_bit; bit <<= 1)
        {
            printf("%d", (*((unsigned char *)huffman_pair->code + huffman_pair->code_length - 1) & bit) != 0);
        }
    }
}

int huffman_encrypt_data(const void * data, const unsigned int data_length, void ** encrypted_data, unsigned int * encrypted_length, struct node ** huffman_root, struct hash_table ** huffman)
{
    if (data == NULL || encrypted_data == NULL || encrypted_length == NULL)
    {
        return NULL_ARGUMENT;
    }

    if ((*huffman = huffman_hash_table(data, data_length, 1, huffman_root, 128, &parse_sequences, &huffman_hash))== NULL)
    {
        return NULL_RESULT;
    }

    int result = STATUS_SUCCESS;
    unsigned int byte_offset = 0;
    unsigned char bit_offset = 1;

    *encrypted_length = 2;
    if ((*encrypted_data = (void *)calloc(*encrypted_length, sizeof(unsigned char))) == NULL)
    {
        result = BAD_MEMORY_ALLOC;
        goto err_exit;
    }

    struct huffman_code * huffman_pair;
    for (unsigned int i = 0; i < data_length; ++i)
    {
        huffman_pair = (struct huffman_code *)find_by_key(*huffman, (unsigned char *)data + i, sizeof(unsigned char), huffman_hash)->root->info.sequence;

        if (huffman_pair == NULL)
        {
            result = NULL_RESULT;
            goto err_exit;
        }

        if (*encrypted_length <= byte_offset + huffman_pair->code_length)
        {
            unsigned int new_size = *encrypted_length << 1;

            if ((*encrypted_data = (void *)realloc(*encrypted_data, sizeof(unsigned char) * new_size)) == NULL)
            {
                result = BAD_MEMORY_ALLOC;
                goto err_exit;
            }

            memset(*encrypted_data + *encrypted_length, 0, *encrypted_length);
            *encrypted_length = new_size;
        }

        for (unsigned int byte = 0; byte < huffman_pair->code_length - 1; ++byte)
        {
            for (unsigned char  bit = 1; bit != 0; bit <<= 1)
            {
                *((unsigned char *)*encrypted_data + byte_offset) |= ((*((unsigned char *)huffman_pair->code + byte)) & bit) != 0 ? bit_offset : 0;

                if (bit_offset == 128)
                {
                    ++byte_offset;
                    bit_offset = 1;
                }
                else
                {
                    bit_offset = bit_offset * 2;
                }
            }
        }

        for (unsigned char bit = 1; bit < huffman_pair->last_bit; bit = bit * 2)
        {
            *((unsigned char *)*encrypted_data + byte_offset) |= ((*((unsigned char *)huffman_pair->code + huffman_pair->code_length - 1)) & bit) != 0 ? bit_offset : 0;

            if (bit_offset == 128)
            {
                ++byte_offset;
                bit_offset = 1;
            }
            else
            {
                bit_offset = bit_offset * 2;
            }
        }
    }

    *encrypted_length = byte_offset + 1;
    *encrypted_data = (void *)realloc(*encrypted_data, *encrypted_length);

    if (*encrypted_data == NULL)
    {
        result = BAD_MEMORY_ALLOC;
        goto err_exit;
    }

    return result;

err_exit:
    clean_nodes(huffman_root);
    clean_table(*huffman);
    free(*huffman_root);
    free(*huffman);
    return result;
}

int huffman_decrypt_data(const void * data, const unsigned int data_length, void ** decrypted_data, unsigned int * decrypted_length, struct node * huffman_root, struct hash_table * huffman_table, const bool print_flag)
{
    if (huffman_root == NULL || decrypted_data == NULL)
    {
        return NULL_ARGUMENT;
    }

    struct node * huffman_node;
    unsigned int byte_index = 0;
    unsigned char bit_offset = 1;

    *decrypted_length = 1;
    *decrypted_data = (void *)malloc(sizeof(unsigned char) * (*decrypted_length));

    if (decrypted_data == NULL)
    {
        return BAD_MEMORY_ALLOC;
    }

    printf("The decrypted data is:\n\n");

    for (unsigned int byte_offset = 0; byte_offset < data_length;)
    {
        for (huffman_node = huffman_root; huffman_node->left_child != NULL && huffman_node->right_child != NULL;)
        {
            if ((*((unsigned char *)data + byte_offset) & bit_offset) == 0)
            {
                huffman_node = huffman_node->left_child;
            }
            else
            {
                huffman_node = huffman_node->right_child;
            }

            if (bit_offset == 128)
            {
                ++byte_offset;
                bit_offset = 1;
            }
            else
            {
                bit_offset <<= 1;
            }
        }

        if (((unsigned char *)huffman_node->info.sequence)[0] == NULL)
        {
            break;
        }

        printf("%c", ((unsigned char *)huffman_node->info.sequence)[0]);

        if (byte_index == *decrypted_length)
        {
            if ((*decrypted_data = (void *)realloc(*decrypted_data, *decrypted_length <<= 1)) == NULL)
            {
                return BAD_MEMORY_ALLOC;
            }
        }

        *((unsigned char *)*decrypted_data + byte_index++) = *(unsigned char *)huffman_node->info.sequence;
    }


    if ((*decrypted_data = (void *)realloc(*decrypted_data, *decrypted_length = byte_index)) == NULL)
    {
        return BAD_MEMORY_ALLOC;
    }

    printf("\n");
    return STATUS_SUCCESS;
}

int encode_huffman_file(const char * input_file_name, const char * output_file_name, struct node ** huffman_root, struct hash_table ** huffman_table, const bool print_flag)
{
    int result = STATUS_SUCCESS;
    void *data;
    unsigned int length;
    void *encrypted_data;
    unsigned int encrypted_length;

    /// READ DATA FROM GIVEN INPUT FILE
    if ((result = fetch_data(input_file_name, &data, &length, "rb")) != STATUS_SUCCESS)
    {
        printf("Could not fetch data from file: %s", input_file_name);
        return result;
    }

    /// ENCODE DATA WITH HUFFMAN-TREE ALGORITHM
    if ((result = huffman_encrypt_data(data, length, &encrypted_data, &encrypted_length, huffman_root, huffman_table)) != STATUS_SUCCESS)
    {
        printf("Could not encode data.");
        free(data);
        return result;
    }
    free(data);

    /// WRITE ENCRYPTED DATA TO OUTPUT FILE
    if ((result = write_data(output_file_name, encrypted_data, encrypted_length)) != STATUS_SUCCESS)
    {
        printf("Could not write encrypted data to file: %s", output_file_name);
        free(encrypted_data);
        return result;
    }

    if (print_flag == true)
    {
        /// VIEW THE SHANNON-INFORMATION
        printf("View the Shannon Entropy for the Huffman encoding:\nPress to continue:"); getc(stdin);
        printf("The Shannon Entropy for the Huffman Coding is: %f\n\n", huffman_entropy(*huffman_root, 0, length));

        /// PRINT THE HUFFMAN TREE
        printf("The Huffman Tree's internal structure:\nPress to continue:"); getc(stdin);
        rsd(*huffman_root, print_node);
        printf("\n\n");

        /// PRINT THE HUFFMAN HASH-TABLE
        printf("View the Huffman codes:\nPress to continue:"); getc(stdin);
        print_table(*huffman_table, print_huffman_code);
        printf("\n");

        /// VIEW THE ENCRYPTED DATA
        printf("View encrypted data:\nPress to continue:"); getc(stdin);
        print_bits(encrypted_data, encrypted_length, FORMAT_BITS, false);
        printf("\n\n");
    }

    free(encrypted_data);
    return result;
}

int decode_huffman_file(const char * input_file_name, const char * output_file_name, struct node * huffman_root, struct hash_table * huffman_table, const bool print_flag)
{
    void * encrypted_data;
    unsigned int encrypted_length;

    /// READ FROM FILE
    FILE* input = fopen(input_file_name, "rb");

    if (input == NULL)
    {
        perror("Could not open file.");
        return FILE_ERROR;
    }

    fseek(input, 0L, SEEK_END);
    encrypted_length = ftell(input);
    encrypted_data = (void *)malloc(encrypted_length * sizeof(unsigned char));

    if (encrypted_data == NULL)
    {
        return BAD_MEMORY_ALLOC;
    }

    fseek(input, 0L, SEEK_SET);
    fread(encrypted_data, sizeof(unsigned char), encrypted_length, input);
    fclose(input);

    int result;
    void * decryped_data;
    unsigned int decrypted_length;

    printf("View decrypted data:\nPress to continue:"); getc(stdin);
    if ((result = huffman_decrypt_data(encrypted_data, encrypted_length, &decryped_data, &decrypted_length, huffman_root, huffman_table, print_flag)) != STATUS_SUCCESS)
    {
        free(encrypted_data);
        return result;
    }

    free(encrypted_data);
    if ((result = write_data(output_file_name, decryped_data, decrypted_length)) != STATUS_SUCCESS)
    {
        printf("Could not write decrypted data to file: %s", output_file_name);
        free(decryped_data);
        return result;
    }
    free(decryped_data);
    return STATUS_SUCCESS;
}
