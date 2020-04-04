#include "utilities.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "node.h"

struct node * node_deep_copy(const struct node * const node)
{
    if (node == NULL)
    {
        return NULL;
    }

    struct node * new_node = (struct node *)malloc(sizeof(struct node));

    if (new_node == NULL)
    {
        return NULL;
    }

    new_node = (struct node *)memcpy(new_node, node, sizeof(struct node));

    if (node->info.sequence != NULL && node->info.length != 0)
    {
        new_node->info.sequence = (void *)malloc(node->info.length);

        if (new_node->info.sequence == NULL)
        {
            free(new_node);
            return NULL;
        }

        new_node->info.sequence = (void *)memcpy(new_node->info.sequence, node->info.sequence, node->info.length);
    }

    return new_node;
}

int destroy_node(struct node ** node)
{
    if (node == NULL || *node == NULL)
    {
        return NULL_ARGUMENT;
    }

    if ((*node)->info.sequence != NULL)
    {
        free((*node)->info.sequence);
    }

    free(*node);
    *node = NULL;

    return STATUS_SUCCESS;
}

void print_node(const struct node * const node)
{
    if (node != NULL)
    {
        if (node->info.sequence == NULL)
        {
            printf("\t\t--- > (INTERMEDIARY): %d\n", node->info.total);
        }
        else
        {
            printf("\t\t--- > |");
            print_bits(node->info.sequence, node->info.length, FORMAT_ASCII, false);
            printf("| LENGTH IS: %d, TOTAL IS: %d\n", node->info.length, node->info.total);
        }
    }
}
