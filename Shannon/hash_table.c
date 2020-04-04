#include "hash_table.h"
#include "avl_tree.h"
#include "utilities.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

int seq_cmp(const void * const sequence_one, const unsigned int sz_one, const void * const sequence_two, const unsigned int sz_two)
{
    int cmp = memcmp(sequence_one, sequence_two, sz_one < sz_two ? sz_one : sz_two);

    if (cmp != 0)
    {
        return cmp;
    }
    else if (sz_one == sz_two)
    {
        return 0;
    }
    else
    {
        return sz_one < sz_two ? -1 : 1;
    }

    return 0;
}

int hash_code(const void * const sequence, unsigned int sz)
{
    if (sequence == NULL || sz == 0)
    {
        return NULL_ARGUMENT;
    }

    int sum = 0;

    do
    {
        --sz;
        sum += *((const char *)sequence + sz);
    }
    while(sz);

    return sum;
}

struct node * const find_by_kv(const struct hash_table* const table, const void * const element, const unsigned int length, int (*hash_fun)(const void * const seq, unsigned int sz), int (*compare)(const void * const, const unsigned int, const void * const, const unsigned int))
{
    if (table == NULL || table->trees == NULL || element == NULL)
    {
        return NULL;
    }

    return find_by_value(find_by_key(table, element, length, hash_fun), element, length, compare);
}

struct avl_tree * const find_by_key(const struct hash_table* const table, const void * const element, const unsigned int length, int (*hash_fun)(const void * const seq, unsigned int sz))
{
    if (table == NULL || table->trees == NULL || element == NULL)
    {
        return NULL;
    }

    return table->trees + hash_fun(element, length) % table->table_size;
}

struct node * const find_first_by_value(const struct hash_table * const table, const void * const element, const unsigned int length, int (*compare)(const void * const, const unsigned int, const void * const, const unsigned int))
{
    if (table == NULL || element == NULL || table->trees == NULL)
    {
        return NULL;
    }

    struct node * first_element;
    for (unsigned int i = 0; i < table->table_size; ++i)
    {
        first_element = find_by_value(table->trees + i, element, length, compare);
        if (first_element != NULL)
        {
            return first_element;
        }
    }

    return NULL;
}

int create_table(struct hash_table* table, const unsigned int table_size)
{
    if (table == NULL)
    {
        return NULL_ARGUMENT;
    }

    if ((table->trees = (struct avl_tree *)calloc(table_size, sizeof(struct avl_tree))) == NULL)
    {
        return BAD_MEMORY_ALLOC;
    }

    table->table_size = table_size;
    table->element_count = 0;

    return STATUS_SUCCESS;
}

int clean_table(struct hash_table* table)
{
    if (table == NULL)
    {
        return NULL_ARGUMENT;
    }

    if (table->trees != NULL)
    {
        int result;
        for (unsigned int i = 0; i < table->table_size; ++i)
        {
            result = clean_tree(table->trees + i);
            if (result != STATUS_SUCCESS)
            {
                return result;
            }
        }

        free(table->trees);
        table->trees = NULL;
    }

    return STATUS_SUCCESS;
}

int add_element(struct hash_table* table, const void * const element, const unsigned int length, int (*hash_fun)(const void * const, unsigned int), int (*compare)(const void * const, const unsigned int, const void * const, const unsigned int))
{
    if (table == NULL || hash_fun == NULL || table->trees == NULL || compare == NULL || element == NULL)
    {
        return NULL_ARGUMENT;
    }

    {
        struct node * const is_found = find_by_kv(table, element, length, hash_fun, compare);

        if (is_found != NULL)
        {
            ++is_found->info.total;
            return STATUS_SUCCESS;
        }
    }

    int result = add_to_tree(table->trees + hash_fun(element, length) % table->table_size, element, length, compare);

    if (result == STATUS_SUCCESS)
    {
        ++table->element_count;
    }

    return result;
}

void print_table(struct hash_table* table, void (*printer)(const struct node * const))
{
    if (table != NULL && table->trees != NULL)
    {
        printf("HASH-TABLE(%p): %d ELEMENTS, %d TABLE-SIZE\n", table, table->element_count, table->table_size);
        for (unsigned int i = 0; i < table->table_size; ++i)
        {
            /// COMMENT IF YOU WANT TO PRINT THE FULL TREE !!!
            if ((table->trees + i)->root != NULL)
            {
                print_tree(table->trees + i, i, printer);
            }
        }
        printf("\n");
    }
}
