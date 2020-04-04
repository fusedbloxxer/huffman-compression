#ifndef _NODE_H_
#define _NODE_H_
#include "weight_data.h"

struct node
{
    /// DATA CONTAINED
    struct weight_data info;

    /// BINDINGS
    int bal;
    struct node* left_child;
    struct node* right_child;
};

int destroy_node(struct node ** node);

/**
*   @PARAMS
*   node    - Memory address of the leaf node
*/
void print_node(const struct node * const node);

struct node * node_deep_copy(const struct node * const node);

#endif // _NODE_H_
