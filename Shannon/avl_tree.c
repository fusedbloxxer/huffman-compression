#include "utilities.h"
#include "avl_tree.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "heap.h"

struct node * const find_by_value(const struct avl_tree * const tree, const void * const element, const unsigned int length, int (*compare)(const void * const, const unsigned int, const void * const, const unsigned int))
{
    if (tree == NULL || element == NULL)
    {
        return NULL;
    }

    return find_element(tree->root, element, length, compare);
}

struct node * const find_element(struct node * const node, const void * const element, const unsigned int length, int (*compare)(const void * const, const unsigned int, const void * const, const unsigned int))
{
    if (node != NULL && element != NULL)
    {
        int cmp = compare(element, length, node->info.sequence, node->info.length);

        if (cmp == 0)
        {
            return node;
        }
        else if (cmp < 0)
        {
            return find_element(node->left_child, element, length, compare);
        }
        else
        {
            return find_element(node->right_child, element, length, compare);
        }
    }
    return NULL;
}

int add_to_tree(struct avl_tree * tree, const void * const element, const unsigned int length, int (*compare)(const void * const, const unsigned int, const void * const, const unsigned int))
{
    if (tree == NULL || element == NULL || compare == NULL)
    {
        return NULL_ARGUMENT;
    }
    ++tree->total;
    return add_node(&tree->root, element, length, compare);
}

int update_tree(struct node * node)
{
    if (node != NULL)
    {
        int left = update_tree(node->left_child);
        int right = update_tree(node->right_child);
        node->bal = left - right;
        return (left < right ? right : left) + 1;
    }
    return 0;
}

void rotate_right_to_left(struct node ** node)
{
        struct node * auxiliary = (*node)->right_child;
        (*node)->right_child = auxiliary->left_child;

        auxiliary->left_child = *node;

        (*node)->bal += 1 - (auxiliary->bal > 0 ? 0 : auxiliary->bal);
        auxiliary->bal += 1 + ((*node)->bal < 0 ? 0 : (*node)->bal);

        *node = auxiliary;
}

void rotate_left_to_right(struct node ** node)
{
        struct node * auxiliary = (*node)->left_child;
        (*node)->left_child = auxiliary->right_child;

        auxiliary->right_child = *node;

        (*node)->bal -= 1 + (auxiliary->bal < 0 ? 0 : auxiliary->bal);
        auxiliary->bal -= 1 - ((*node)->bal > 0 ? 0 : (*node)->bal);

        *node = auxiliary;
}

int add_node(struct node ** node, const void * const element, const unsigned int length, int (*compare)(const void * const, const unsigned int, const void * const, const unsigned int))
{
    if (node == NULL)
    {
        return NULL_ARGUMENT;
    }

    if (*node == NULL)
    {
        *node = (struct node *)malloc(sizeof(struct node));

        if (*node == NULL)
        {
            return BAD_MEMORY_ALLOC;
        }

        /// INITIALIZE DATA
        (*node)->bal = 0;
        (*node)->info.total = 1;

        /// INITIALIZE BINDINGS
        (*node)->left_child = NULL;
        (*node)->right_child = NULL;

        /// COPY DATA
        (*node)->info.length = length;
        (*node)->info.sequence = (void *)malloc(length);
        (*node)->info.sequence = (void *)memcpy((*node)->info.sequence, element, length);
    }
    else
    {
        int cmp = compare(element, length, (*node)->info.sequence, (*node)->info.length);

        if (cmp == 0)
        {
            ++(*node)->info.total;
        }
        else if (cmp < 0)
        {
            add_node(&(*node)->left_child, element, length, compare);
            update_tree(*node);

            if (abs((*node)->bal) > 1)
            {
                if (((*node)->bal > 0) - ((*node)->bal < 0) != ((*node)->left_child->bal > 0) - ((*node)->left_child->bal < 0))
                {
                    rotate_right_to_left(&(*node)->left_child);
                }
                rotate_left_to_right(node);
            }
        }
        else
        {
            add_node(&(*node)->right_child, element, length, compare);
            update_tree(*node);

            if (abs((*node)->bal) > 1)
            {
                if (((*node)->bal > 0) - ((*node)->bal < 0) != ((*node)->right_child->bal > 0) - ((*node)->right_child->bal < 0))
                {
                    rotate_left_to_right(&(*node)->right_child);
                }
                rotate_right_to_left(node);
            }
        }
    }
    return STATUS_SUCCESS;
}

void print_tree(const struct avl_tree * const tree, const unsigned int index, void (*printer)(const struct node * const))
{
    if (tree != NULL)
    {
        printf(" ---* TREE(%p): %d HASH-CODE, %d ELEMENTS\n", tree, index, tree->total);
        srd(tree->root, printer);
        printf("\n");
    }
}

void srd(const struct node * const node, void (*executer)(const struct node * const))
{
    if (node != NULL && executer != NULL)
    {
        if (node->left_child != NULL)
        {
            srd(node->left_child, executer);
        }

        executer(node); printf("\n");

        if (node->right_child != NULL)
        {
            srd(node->right_child, executer);
        }
    }
}

void rsd(const struct node * const node, void (*executer)(const struct node * const))
{
    if (node != NULL && executer != NULL)
    {
        executer(node);

        if (node->left_child != NULL)
        {
            srd(node->left_child, executer);
        }

        if (node->right_child != NULL)
        {
            srd(node->right_child, executer);
        }
    }
}

void print_nodes_default(const struct node * const node)
{
    srd(node, print_node);
}

int clean_tree(struct avl_tree* tree)
{
    if (tree != NULL)
    {
        int result = clean_nodes(&tree->root);

        if (result != STATUS_SUCCESS)
        {
            return result;
        }

        tree->total = 0;
    }
    return STATUS_SUCCESS;
}

int clean_nodes(struct node ** node)
{
    if (*node != NULL)
    {
        if ((*node)->left_child != NULL)
        {
            clean_nodes(&(*node)->left_child);
        }

        if ((*node)->right_child != NULL)
        {
            clean_nodes(&(*node)->right_child);
        }

        return destroy_node(node);
    }
    return STATUS_SUCCESS;
}
