#include <stdio.h>
#include "utilities.h"
#include "shannon.h"
#include "huffman.h"
#include "heap.h"

int main()
{
    /// SHANNON-INFORMATION-EXERCISE:

    int result;
    void * buffer;
    unsigned int buffer_length;
    const char * input_file = "plain_text.txt";

    FILE * input = fopen(input_file, "r");

    if (input == NULL)
    {
        perror("Could not open file.");
        return FILE_ERROR;
    }

    fseek(input, 0, SEEK_END);
    buffer_length = ftell(input);
    fseek(input, 0, SEEK_SET);

    if ((buffer = (void *)malloc(sizeof(unsigned char) * buffer_length)) == NULL)
    {
        perror("Could not allocate memory for the buffer.");
        return BAD_MEMORY_ALLOC;
    }

    fread(buffer, sizeof(unsigned char), buffer_length, input);
    fclose(input);

    printf("Shannon Information for 1-sized sequences: ");
    printf("\nPress key to continue: "); getc(stdin);
    printf("Shannon Information for 1-sized sequences: %f\n", sequence_entropy(buffer, buffer_length, 1, 1));

    printf("\nShannon Information for 2-sized sequences: ");
    printf("\nPress key to continue: "); getc(stdin);
    printf("Shannon Information for 2-sized sequences: %f\n", sequence_entropy(buffer, buffer_length, 2, 3));

    printf("\nShannon Information for 3-sized sequences: ");
    printf("\nPress key to continue: "); getc(stdin);
    printf("Shannon Information for 3-sized sequences: %f\n", sequence_entropy(buffer, buffer_length, 3, 5));

    printf("\nShannon Information for words: ");
    printf("\nPress key to continue: "); getc(stdin);
    printf("Shannon Information for words: %f\n", words_entropy(buffer, buffer_length, 1, 100));
    free(buffer);

    /// HUFFMAN-ENCRYPTION-EXERCISE:

    struct node * huffman_root;
    struct hash_table * huffman_table;

    const char * output_file = "encrypted_text.txt";
    const char * result_file = "decrypted_text.txt";

    if ((result = encode_huffman_file(input_file, output_file, &huffman_root, &huffman_table, true)) != STATUS_SUCCESS)
    {
        return result;
    }

    if ((result = decode_huffman_file(output_file, result_file, huffman_root, huffman_table, true)) != STATUS_SUCCESS)
    {
        return result;
    }

    clean_nodes(&huffman_root);
    clean_table(huffman_table);
    free(huffman_table);
    free(huffman_root);
    return 0;
}
