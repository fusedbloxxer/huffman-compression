#include "hash_table.h"
#include "utilities.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

struct hash_table * frequency_hash_table(const void * const data, const unsigned int length, const unsigned int specifier, const unsigned int table_size, int (*parse_data)(struct hash_table*, const void * const data, const unsigned int length, const unsigned int specifier))
{
    struct hash_table * table = (struct hash_table *)malloc(sizeof(struct hash_table));

    if (create_table(table, table_size) != STATUS_SUCCESS)
    {
        return NULL;
    }

    if (parse_data(table, data, length, specifier) != STATUS_SUCCESS)
    {
        return NULL;
    }

    return table;
}

double node_entropy(const struct node * const node, const unsigned int total)
{
    if (node == NULL)
    {
        return 0;
    }
    double probability = node->info.total / (double) total;
    return - probability * log(probability) / log(2.0);
}

double nodes_entropy(const struct node * const node, const unsigned int total)
{
    if (node != NULL)
    {
        return node_entropy(node, total) +
               nodes_entropy(node->left_child, total) +
               nodes_entropy(node->right_child, total);
    }
    return 0;
}

double tree_entropy(const struct avl_tree* const tree, const unsigned int total)
{
    if (tree != NULL)
    {
        return nodes_entropy(tree->root, total);
    }
    return 0;
}

double hash_table_entropy(const struct hash_table* const table, const unsigned int total)
{
    if (table != NULL && table->trees != NULL)
    {
        double entropy = 0.0;
        for (unsigned int i = 0; i < table->table_size; ++i)
        {
            entropy += tree_entropy(table->trees + i, total);
        }
        return entropy;
    }
    return 0;
}

double shannon_entropy(const char * const filePath, const unsigned int specifier, double (*event)(const void * const data, const unsigned int length, const unsigned int specifier))
{
    unsigned int length;
    void * buffer;

    if (fetch_data(filePath, &buffer, &length, "r") != STATUS_SUCCESS)
    {
        return FILE_ERROR;
    }

    double entropy = event(buffer, length, specifier);
    free((void *)buffer);
    return entropy;
}

int parse_sequences(struct hash_table * table, const void * const data, const unsigned int length, const unsigned int specifier)
{
    if (table == NULL || data == NULL)
    {
        return NULL_ARGUMENT;
    }

    int result;
    for (unsigned int i = 0; i < length - specifier + 1; ++i)
    {
        result = add_element(table, (unsigned char *)data + i, specifier, hash_code, seq_cmp);
        if (result != STATUS_SUCCESS)
        {
            return result;
        }
    }

    /// ADD END-OF-FILE-CHARACTER

    return STATUS_SUCCESS;
}

double sequence_entropy(const void * const data, const unsigned int length, const unsigned int specifier, const unsigned int table_size)
{
    double entropy = 0.0;
    struct hash_table * table = frequency_hash_table(data, length, specifier, table_size, &parse_sequences);

    entropy = hash_table_entropy(table, length - specifier + 1);

    print_table(table, print_node);

    clean_table(table);
    free(table);

    return entropy;
}

int parse_words(struct hash_table * table, const void * const data, const unsigned int length, const unsigned int specifier)
{
    if (table == NULL || data == NULL)
    {
        return NULL_ARGUMENT;
    }

    for (unsigned int i = 0; i < length; ++i)
    {
        if (*((unsigned char *)data + i) != ' ')
        {
            unsigned int j = i + 1;
            while (j < length && *((unsigned char *)data + j) != ' ')
            {
                ++j;
            }

            int result = add_element(table, (unsigned char *)data + i, j - i, hash_code, seq_cmp);
            if (result != STATUS_SUCCESS)
            {
                return result;
            }

            i = j;
        }
    }

    return STATUS_SUCCESS;
}

double words_entropy(const void * const data, const unsigned int length, const unsigned int specifier, const unsigned int table_size)
{
    double entropy = 0.0;
    struct hash_table * table = frequency_hash_table(data, length, specifier, table_size, &parse_words);

    print_table(table, print_node);

    entropy = hash_table_entropy(table, table->element_count);

    clean_table(table);
    free(table);

    return entropy;
}
